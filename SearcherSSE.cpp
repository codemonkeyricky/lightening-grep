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

    alignas( 32 ) char stridedPattern[ 32 ][ 32 ];
    __m256i patternStrided256[ 32 ];

    for ( auto i = 0; i < 32; i++ )
    {
        memset( stridedPattern[ i ], 0, 32 );

        memcpy( &stridedPattern[ i ][ i ], pattern.c_str(), 32 - i );

        patternStrided256[ i ] = _mm256_loadu_si256( ( const __m256i * ) &stridedPattern[ i ] );
    }

    for ( auto k = 0; k < size; k += MMAP_SIZE )
    {
        char * memoryMap = ( char * ) mmap( 0, MMAP_SIZE, PROT_READ, MAP_SHARED, fd, k );

//        // NOTE: _SIDD_LEAST_SIGNIFICANT sets the same bit as _SIDD_BIT_MASK
//        // const int mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_UNIT_MASK;
//        const int mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH;

        for ( auto i = 0; i < MMAP_SIZE; i += 32 )
        {
            auto octadword  = _mm256_load_si256( ( const __m256i * ) ( memoryMap + i ) );

            auto result     = _mm256_cmpeq_epi8( octadword, firstLetterRepated );
            auto resultMask = _mm256_movemask_epi8( result );

            if ( resultMask )
            {
                unsigned int m  = resultMask;
                int lsbPos      = ffs( m ) - 1;
                auto r          = _mm256_cmpeq_epi8( octadword, patternStrided256[ lsbPos ] );
                auto m2         = _mm256_movemask_epi8( r );
                auto c          = _mm_popcnt_u32( m2 );

                if ( c == ( 32 - lsbPos ) )
                {
                    if ( c > 5 )
                        count++;
                }
            }
        }

        munmap( memoryMap, MMAP_SIZE );
    }

    return count;
}
