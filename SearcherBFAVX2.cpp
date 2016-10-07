#include "SearcherBFAVX2.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <immintrin.h>
#include "immintrin.h"
#include <assert.h>

#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

SearcherBFAVX2::SearcherBFAVX2()
{

}


SearcherBFAVX2::~SearcherBFAVX2()
{

}


vector< SearcherI::Instance > SearcherBFAVX2::process(
    string & filename,
    string & pattern
    )
{
    int fd      = open( filename.c_str(), O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s    = pattern;

    // Limit for now.
    assert( pattern.size() <= 32 );

    vector< Instance > summary;
    int ln = 0;

    int count = 0;

    alignas( 32 ) char first8bitsRepeated[ 32 ];
    memset( first8bitsRepeated, pattern[ 0 ], 32 );
    auto firstLetterRepated = _mm256_loadu_si256( ( const __m256i * ) &first8bitsRepeated[ 0 ] );

    alignas( 32 ) char nl[ 32 ];
    memset( nl, '\n', 32 );
    auto nl256 = _mm256_loadu_si256( ( const __m256i * ) nl );

    alignas( 32 ) char sp1[ 32 ][ 32 ];
    alignas( 32 ) char sp2[ 32 ][ 32 ];
    __m256i sp1_256[ 32 ];
    __m256i sp2_256[ 32 ];

    memset( sp1, 0, sizeof( sp1 ) );
    memset( sp2, 0, sizeof( sp2 ) );

    for ( auto i = 0; i < 32; i++ )
    {
        memset( sp1[ i ], 0, 32 );

        int buffer_size = 32 - i;
        int to_copy = ( pattern.size() < buffer_size ) ?
            pattern.size() : buffer_size;

        memcpy( &sp1[ i ][ i ], pattern.c_str(), to_copy );
        sp1_256[ i ] = _mm256_loadu_si256( ( const __m256i * ) &sp1[ i ] );

        to_copy = ( i < pattern.size() ) ?
            ( pattern.size() - i ) : 0;

        memcpy( &sp2[ i ][ 0 ], pattern.c_str() + i, to_copy );
        sp2_256[ i ] = _mm256_loadu_si256( ( const __m256i * ) &sp2[ i ] );
    }

    for ( auto offset = 0; offset < size; offset += MMAP_SIZE )
    {
        auto ms = std::min( MMAP_SIZE + 32, size - offset );

        char * mm = ( char * ) mmap( 0, ms, PROT_READ, MAP_SHARED, fd, offset );
        assert( mm != ( void * ) -1 );

        for ( auto i = 0; i < ms; i += 32 )
        {
            auto char32     = _mm256_load_si256( ( const __m256i * ) ( mm + i ) );
            auto curr       = char32;

            auto result     = _mm256_cmpeq_epi8( char32, firstLetterRepated );
            auto resultMask = _mm256_movemask_epi8( result );

            if ( resultMask )
            {
                unsigned int m  = resultMask;
                while ( m )
                {
                    int mi  = ffs( m ) - 1;
                    m       &= m-1;

                    auto r  = _mm256_cmpeq_epi8( char32, sp1_256[ mi ] );
                    auto m2 = _mm256_movemask_epi8( r );
                    int c   = _mm_popcnt_u32( m2 );

                    int mreq =
                        ( 32 - mi ) > pattern.size() ?
                            pattern.size() : 32 - mi;

                    if ( c == mreq )
                    {
                        if ( mreq == pattern.size() )
                        {
                            // Pattern found.

                            // Switch to scalar code for better maintenance.

                            char *pos = strstr( mm + i, pattern.c_str() );

                            if ( pos == 0 )
                                continue;

                            // Find the newlines preceding the pattern.
                            int line = ln;
                            char *curr = mm + i;
                            while ( curr++ != pos )
                                line += ( *curr == '\n' );

                            char *start, *end;
                            start = end = pos;

                            // Find the previous newline.
                            while ( *(--start) != '\n' )
                            { }
                            start++;

                            // Find the end newline.
                            while ( *(++end) != '\n' )
                            { }

                            auto content = string( start, end - start );
                            summary.emplace_back(
                                line + 1,
                                pos - start,
                                content
                                );
                        }
                        else
                        {
                            int remain = pattern.size() - ( 32 - mi );

                            char32  = _mm256_load_si256( ( const __m256i * ) ( mm + i + 32 ) );
                            r       = _mm256_cmpeq_epi8( char32, sp2_256[ mreq ] );
                            m2      = _mm256_movemask_epi8( r );
                            c       = _mm_popcnt_u32( m2 );

                            if ( c == remain )
                            {
                                string nothing;
                                summary.emplace_back(
                                    ln + 1,
                                    0,
                                    nothing
                                );
                            }
                        }
                    }
                }
            }

            auto nls    = _mm256_cmpeq_epi8( curr, nl256 );
            auto nlm    = _mm256_movemask_epi8( nls );
            auto lines  = _mm_popcnt_u32( nlm );

            ln  += lines;
        }

        munmap( mm, ms );
    }

//    printf( "lineCount = %d\n", ln );

    close( fd );

    return summary;
}
