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

#include "SearcherSSE.hpp"

using namespace std;

SearcherSSE::SearcherSSE()
{

}


SearcherSSE::~SearcherSSE()
{

}


uint32_t SearcherSSE::process(
    string & filename,
    string & pattern
    )
{
    int fd      = open( filename.c_str(), O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s    = pattern;

    // Limit for now.
    assert( pattern.size() <= 32 );

    int count = 0;

    alignas( 32 ) char first8bitsRepeated[ 32 ];
    memset( first8bitsRepeated, pattern[ 0 ], 32 );
    auto firstLetterRepated = _mm256_loadu_si256( ( const __m256i * ) &first8bitsRepeated[ 0 ] );

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

    for ( auto k = 0; k < size; k += MMAP_SIZE )
    {
        char * mm = ( char * ) mmap( 0, MMAP_SIZE, PROT_READ, MAP_SHARED, fd, k );

//        // NOTE: _SIDD_LEAST_SIGNIFICANT sets the same bit as _SIDD_BIT_MASK
//        // const int mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_UNIT_MASK;
//        const int mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH;

        for ( auto i = 0; i < MMAP_SIZE; i += 32 )
        {
            auto octadword  = _mm256_load_si256( ( const __m256i * ) ( mm + i ) );

            auto result     = _mm256_cmpeq_epi8( octadword, firstLetterRepated );
            auto resultMask = _mm256_movemask_epi8( result );

            if ( resultMask )
            {
                unsigned int m  = resultMask;
                int matchIndex  = ffs( m ) - 1;
                auto r          = _mm256_cmpeq_epi8( octadword, sp1_256[ matchIndex ] );
                auto m2         = _mm256_movemask_epi8( r );
                int c           = _mm_popcnt_u32( m2 );

                int matchRequired =
                    ( 32 - matchIndex ) > pattern.size() ?
                        pattern.size() : 32 - matchIndex;

                if ( c == matchRequired )
                {
                    if ( matchRequired == pattern.size() )
                    {
                        count++;
                    }
                    else
                    {
                        int remaining = pattern.size() - ( 32 - matchIndex );

                        octadword  = _mm256_load_si256( ( const __m256i * ) ( mm + i + 32 ) );
                        r          = _mm256_cmpeq_epi8( octadword, sp2_256[ remaining ] );
                        m2         = _mm256_movemask_epi8( r );
                        c          = _mm_popcnt_u32( m2 );

                        if ( c == remaining )
                        {
                            count++;
                        }
                    }
                }
            }
        }

        munmap( mm, MMAP_SIZE );
    }

    return count;
}
