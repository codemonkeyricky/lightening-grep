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

    int count = 0;

    for ( auto k = 0; k < size; k += MMAP_SIZE )
    {
        auto data = mmap( 0, MMAP_SIZE, PROT_READ, MAP_SHARED, fd, k );

        char * t = ( char * ) data;

        __m128i input, b;

        // NOTE: _SIDD_LEAST_SIGNIFICANT sets the same bit as _SIDD_BIT_MASK
        // const int mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_UNIT_MASK;
        const int mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH;

        alignas( 32 ) char first8bitsRepeated[ 16 ];
        memset( first8bitsRepeated, pattern[ 0 ], 16 );
        auto first = _mm_loadu_si128( ( const __m128i * ) &first8bitsRepeated[ 0 ] );

        alignas( 32 ) char stridedPattern[ 16 ][ 16 ];
        __m128i stridedPattern128[ 16 ];
        for ( auto i = 0; i < 16; i++ )
        {
            memset( stridedPattern[ i ], 0, 16 );

            memcpy( &stridedPattern[ i ][ i ], pattern.c_str(), 16 - i );

            stridedPattern128[ i ] = _mm_loadu_si128( ( const __m128i * ) &stridedPattern[ i ] );
        }

        auto i = 0;
//        while ( i < MMAP_SIZE )
        for ( auto i = 0; i < 4096; i += 16 )
        {
            input = _mm_load_si128( ( const __m128i * ) ( t + i ) );

            auto result = _mm_cmpeq_epi8( input, first );
            auto mask = _mm_movemask_epi8( result );

            if ( mask )
            {
                int pos = 0;
                auto m = mask;
                while ( m )
                {
                    m >>= 1;

                    pos++;
                }
                --pos;

                auto r = _mm_cmpeq_epi8( input, stridedPattern128[ pos ] );
                auto m2 = _mm_movemask_epi8( r );
                auto c = _mm_popcnt_u32( m2 );
                if ( c == ( 16 - pos ) )
                {
                    if ( c > 5 )
                        count++;
                }
            }
        }

        munmap( data, MMAP_SIZE );
    }

    return count;
}
