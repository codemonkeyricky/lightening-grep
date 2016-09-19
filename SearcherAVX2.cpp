#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <immintrin.h>
#include <assert.h>

#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>


#include "SearcherAVX2.hpp"

using namespace std;

SearcherAVX2::SearcherAVX2()
{

}


SearcherAVX2::~SearcherAVX2()
{

}


static int kMod        = 0x800000;
static int kBase       = 129;
#define MMAP_SIZE 4096

void vsearch(
    int     streams,
    int     input[ MMAP_SIZE ],
    __m256i output[ 512 ],
    int targetHash,
    int slen,
    int p
    )
{
    int streamSize          = MMAP_SIZE / streams;

    __m256i zeroes          = _mm256_set_epi32( 0, 0, 0, 0, 0, 0, 0, 0 );
    __m256i currHash        = zeroes;
    __m256i targetHash128   = _mm256_set_epi32( targetHash, targetHash, targetHash, targetHash, targetHash, targetHash, targetHash, targetHash );
    __m256i modMask         = _mm256_set_epi32( kMod - 1, kMod - 1, kMod - 1, kMod - 1, kMod - 1, kMod - 1, kMod - 1, kMod - 1 );

    __m256i power256        = _mm256_set_epi32( p, p, p, p, p, p, p, p );
    __m256i kBase256        = _mm256_set_epi32( kBase, kBase, kBase, kBase, kBase, kBase, kBase, kBase );
    __m256i kMod256         = _mm256_set_epi32( kMod, kMod, kMod, kMod, kMod, kMod, kMod, kMod );
    __m256i rollingHash256  = zeroes;

    for ( auto i = 0; i < slen; i++ )
    {
        // thash = ( thash * kBase + t[ i ] ) % kMod;

        auto tadd       = _mm256_set_epi32(
            input[ streamSize * 0 + i ],
            input[ streamSize * 1 + i ],
            input[ streamSize * 2 + i ],
            input[ streamSize * 3 + i ],
            input[ streamSize * 4 + i ],
            input[ streamSize * 5 + i ],
            input[ streamSize * 6 + i ],
            input[ streamSize * 7 + i ] );

        auto a          = _mm256_mul_epi32( rollingHash256, kBase256 );
        auto b          = _mm256_add_epi32( a, tadd );

        rollingHash256  = _mm256_and_si256( b, modMask );
    }

    for ( auto i = slen; i < ( MMAP_SIZE / streams ); i++ )
    {
        // if ( rollingHash == targetHash ) { }

        auto result         = _mm256_cmpeq_epi32( rollingHash256, targetHash128 );
        output[ i ]         = result;

        // thash -= ( t[ i - s.size() ] * power ) % kMod;

        // printf( "%c", input[ 1 ] );
        auto remove         = _mm256_set_epi32(
            input[ 512 * 0 + i - slen ],
            input[ 512 * 1 + i - slen ],
            input[ 512 * 2 + i - slen ],
            input[ 512 * 3 + i - slen ],
            input[ 512 * 4 + i - slen ],
            input[ 512 * 5 + i - slen ],
            input[ 512 * 6 + i - slen ],
            input[ 512 * 7 + i - slen ] );

        auto a              = _mm256_mullo_epi32( remove, power256 );
        auto b              = _mm256_and_si256( a, modMask );
        rollingHash256      = _mm256_sub_epi32( rollingHash256, b );

        // if ( thash < 0 ) thash += kMod;

        auto cmp            = _mm256_cmpgt_epi32( zeroes, rollingHash256 );
        auto to_add         = _mm256_and_si256( kMod256, cmp );
        rollingHash256      = _mm256_add_epi32( rollingHash256, to_add );

        // thash = ( thash * kBase + t[ i ] ) % kMod;

//        auto tadd           = input[ i ];
        auto tadd       = _mm256_set_epi32(
            input[ streamSize * 0 + i ], input[ streamSize * 1 + i ], input[ streamSize * 2 + i ], input[ streamSize * 3 + i ],
            input[ streamSize * 4 + i ], input[ streamSize * 5 + i ], input[ streamSize * 6 + i ], input[ streamSize * 7 + i ] );

        a                   = _mm256_mullo_epi32( rollingHash256, kBase256 );
        b                   = _mm256_add_epi32( a, tadd );

        rollingHash256      = _mm256_and_si256( b, modMask );
    }
}


uint32_t SearcherAVX2::process(
    string & filename,
    string & pattern
    )
{
    int fd      = open( filename.c_str(), O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s    = pattern;

    for ( auto k = 0; k < size; k += MMAP_SIZE )
    {
        auto data = mmap( 0, MMAP_SIZE, PROT_READ, MAP_SHARED, fd, k );

        char * t = ( char * ) data;

        int targetHash = 0;
        int rollingHash = 0;
        int power = 1;

        for ( auto i = 0; i < s.size(); i++ )
        {
            power       = i ? power * kBase % kMod : 1;
            rollingHash = ( rollingHash * kBase + t[ i ] ) % kMod;
            targetHash  = ( targetHash * kBase + s[ i ] ) % kMod;
        }

        int streams     = 8;
        int streamSize  = MMAP_SIZE / streams;

        int     remap[ MMAP_SIZE  ];
        for ( auto i = 0; i < MMAP_SIZE; i++ )
        {
            //            int *input = ( int * ) &remap[ i ];
            //
            //            for ( auto j = 0; j < streams; j++ )
            //            {
            //                input[ j ] = t[ streamSize * j + i ];
            //            }
            remap[ i ] = t[ i ];
        }

        __m256i output[ MMAP_SIZE / streams ];
        vsearch( streams, remap, output, targetHash, s.size(), power );


        munmap( data, MMAP_SIZE );
    }
}
