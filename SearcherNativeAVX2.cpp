#include "SearcherNativeAVX2.hpp"

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

SearcherNativeAVX2::SearcherNativeAVX2()
{

}


SearcherNativeAVX2::~SearcherNativeAVX2()
{

}


vector< SearcherI::sMatchInstance > SearcherNativeAVX2::process(
    string & filename,
    string & pattern
    )
{
    auto insertRecord = [] (
        const char   *curr,
        const char   *pattern,
        int     currline,
        vector< sMatchInstance > &summary
        )
    {
        // Pattern found.

        // Switch to scalar code for better maintenance.

        static int lastInsertedLn = -1;

        auto pos = strstr( curr, pattern );

        if ( pos == 0 )
            return;

        // Find the newlines preceding the pattern.
        int line = currline;
        while ( curr++ != pos )
            line += ( *curr == '\n' );

        auto start = pos;
        auto end = pos;

        // Find the previous newline.
        while ( *(--start) != '\n' )
        { }
        start++;

        // Find the end newline.
        while ( *(++end) != '\n' )
        { }

        auto content = string( start, end - start );

        // Multiple matches on one line only needs to be inserted once.
        if ( line + 1 != lastInsertedLn )
        {
            summary.emplace_back(
                line + 1,
                pos - start,
                content
            );
        }

        lastInsertedLn = line + 1;
    };

    int fd      = open( filename.c_str(), O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s    = pattern;
    int patternSize = pattern.size();

    // Limit for now.
    assert( pattern.size() <= 32 );

    vector< sMatchInstance > summary;
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
        // First half.

        memset( sp1[ i ], 0, 32 );

        int buffer_size = 32 - i;
        int to_copy = ( pattern.size() < buffer_size ) ?
            pattern.size() : buffer_size;

        memcpy( &sp1[ i ][ i ], pattern.c_str(), to_copy );
        sp1_256[ i ] = _mm256_loadu_si256( ( const __m256i * ) &sp1[ i ] );

        // Second half.

        to_copy = ( i < pattern.size() ) ?
            ( pattern.size() - i ) : 0;

        memcpy( &sp2[ i ][ 0 ], pattern.c_str() + i, to_copy );
        sp2_256[ i ] = _mm256_loadu_si256( ( const __m256i * ) &sp2[ i ] );
    }

    char mm[ MMAP_SIZE ];
    for ( auto offset = 0; offset < size; offset += MMAP_SIZE )
    {
        auto ms = std::min( MMAP_SIZE + 32, size - offset );

        lseek( fd, offset, SEEK_SET );
        auto rd = read( fd, mm, ms );
        assert( ms == rd );

        for ( auto i = 0; i < ms; i += 32 )
        {
            auto char32     = _mm256_load_si256( ( const __m256i * ) ( mm + i ) );
            auto curr       = char32;

            auto match      = _mm256_cmpeq_epi8( char32, firstLetterRepated );
            auto matchMask  = _mm256_movemask_epi8( match );

            // Iterate through all mask bits.
            while ( matchMask )
            {
                // Find the bit offset.
                int patternOffset   = ffs( matchMask ) - 1;

                int bytesCompared   = 0;
                int iteration       = 0;
                while ( bytesCompared < patternSize )
                {
                    auto r              = _mm256_cmpeq_epi8(
                        char32,
                        ( iteration == 0 ) ?
                            sp1_256[ patternOffset ] : sp2_256[ patternOffset ] );

                    auto m2             = _mm256_movemask_epi8( r );
                    int matchedBytes    = _mm_popcnt_u32( m2 );

                    // Find matching letters required.
                    int bytesRemaining = patternSize - bytesCompared;

                    int bytesRequiredToMatch = ( iteration == 0 ) ?
                        std::min( 32 - patternOffset, patternSize ) : bytesRemaining;

                    // Failed to match.
                    if ( matchedBytes < bytesRequiredToMatch )
                    {
                        break;
                    }
                    else if ( matchedBytes > bytesRequiredToMatch )
                    {
                        auto of = i + iteration * 32 + 32;
                        if ( ms < of )
                        {
                            matchedBytes -= of - ms;

                            // Loaded beyond end of file. False positives may hit due to null characters.
                            // Adjust matchedBytes accordingly.
                        }
                    }

                    // Accounting.
                    bytesCompared += matchedBytes;

                    // If all bytes match, then we found the string.
                    if ( bytesCompared == patternSize )
                    {
                        insertRecord( mm + i, pattern.c_str(), ln, summary );

                        break;
                    }

                    iteration ++;

                    // Load the next 32 bytes.
                    char32  = _mm256_load_si256( ( const __m256i * ) ( mm + i + iteration * 32 ) );

                    patternOffset = bytesRequiredToMatch;
                }

                // Remove the lsb from mask.
                matchMask   &= matchMask - 1;
            }

            auto nls    = _mm256_cmpeq_epi8( curr, nl256 );
            auto nlm    = _mm256_movemask_epi8( nls );
            auto lines  = _mm_popcnt_u32( nlm );

            ln  += lines;
        }

        munmap( mm, ms );
    }

    close( fd );

    return summary;
}
