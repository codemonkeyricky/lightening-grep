#include "SearcherRabinKarpAVX2.hpp"

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


using namespace std;

SearcherAVX2::SearcherAVX2()
{

}


SearcherAVX2::~SearcherAVX2()
{

}


static int kMod        = 0x800000;
static int kBase       = 129;
#define MMAP_SIZE       4096

int vsearch(
    int     streams,
    int     input[ MMAP_SIZE ],
    int     targetHash,
    int     slen,
    int     power
    )
{
    int output[ MMAP_SIZE / streams ];

    int streamSize          = MMAP_SIZE / streams;

    __m256i zeroes256       = _mm256_set_epi32( 0, 0, 0, 0, 0, 0, 0, 0 );
    __m256i ones256         = _mm256_set_epi32( 1, 1, 1, 1, 1, 1, 1, 1 );
    __m256i stridedOnes     = _mm256_set_epi32(
        0x00000001, 0x00000010, 0x00000100, 0x00001000,
        0x00010000, 0x00100000, 0x01000000, 0x10000000 );
    __m256i targetHash128   = _mm256_set_epi32( targetHash, targetHash, targetHash, targetHash, targetHash, targetHash, targetHash, targetHash );
    __m256i modMask256      = _mm256_set_epi32( kMod - 1, kMod - 1, kMod - 1, kMod - 1, kMod - 1, kMod - 1, kMod - 1, kMod - 1 );

    __m256i power256        = _mm256_set_epi32( power, power, power, power, power, power, power, power );
    __m256i kBase256        = _mm256_set_epi32( kBase, kBase, kBase, kBase, kBase, kBase, kBase, kBase );
    __m256i kMod256         = _mm256_set_epi32( kMod, kMod, kMod, kMod, kMod, kMod, kMod, kMod );
    __m256i rollingHash256  = zeroes256;

    vector< int > matches;
    int count = 0;

    matches.reserve( 100 );

    auto addCharToHash = [ & ] ( int i )
    {
        // thash = ( thash * kBase + t[ i ] ) % kMod;

        auto tadd       = _mm256_set_epi32(
            input[ streamSize * 0 + i ], input[ streamSize * 1 + i ], input[ streamSize * 2 + i ], input[ streamSize * 3 + i ],
            input[ streamSize * 4 + i ], input[ streamSize * 5 + i ], input[ streamSize * 6 + i ], input[ streamSize * 7 + i ] );

        auto a          = _mm256_mullo_epi32( rollingHash256, kBase256 );
        auto b          = _mm256_add_epi32( a, tadd );

        rollingHash256  = _mm256_and_si256( b, modMask256 );
    };

    auto removeCharFromHash = [ & ] ( int i )
    {
        // thash -= ( t[ i - s.size() ] * power ) % kMod;

        auto remove         = _mm256_set_epi32(
            input[ 512 * 0 + i  ], input[ 512 * 1 + i  ], input[ 512 * 2 + i  ], input[ 512 * 3 + i  ],
            input[ 512 * 4 + i  ], input[ 512 * 5 + i  ], input[ 512 * 6 + i  ], input[ 512 * 7 + i  ] );

        auto a              = _mm256_mullo_epi32( remove, power256 );
        auto b              = _mm256_and_si256( a, modMask256 );
        rollingHash256      = _mm256_sub_epi32( rollingHash256, b );
    };

    auto ensurePositive = [ & ] ()
    {
        // if ( thash < 0 ) thash += kMod;

        auto cmp            = _mm256_cmpgt_epi32( zeroes256, rollingHash256 );
        auto to_add         = _mm256_and_si256( kMod256, cmp );
        rollingHash256      = _mm256_add_epi32( rollingHash256, to_add );
    };

    auto updateResult = [ & ] ( int i )
    {
        auto result = _mm256_cmpeq_epi32( rollingHash256, targetHash128 );
        result      = _mm256_and_si256( result, stridedOnes );

        result      = _mm256_hadd_epi32( result, result );
        result      = _mm256_hadd_epi32( result, result );

        output[ i ] = _mm256_extract_epi32( result, 0 );
    };

    for ( auto i = 0; i < slen; i++ )
    {
        addCharToHash( i );
    }

    for ( auto i = slen; i < ( MMAP_SIZE / streams ); i++ )
    {
        updateResult( i );

        removeCharFromHash( i - slen );

        ensurePositive();

        addCharToHash( i );
    }

    int * o = ( int * ) output;
    for ( int i = 0; i < MMAP_SIZE / streams; i++ )
    {
        if ( o[ i ] )
        {
            count++;
        }
    }

    return count;
}


vector< SearcherI::Instance > SearcherAVX2::process(
    string & filename,
    string & pattern
    )
{
    int fd      = open( filename.c_str(), O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s    = pattern;

    int count = 0;

    vector< SearcherI::Instance >   result;

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

        int remap[ MMAP_SIZE ];
        for ( auto i = 0; i < MMAP_SIZE; i++ )
        {
            remap[ i ] = t[ i ];
        }

        int output[ MMAP_SIZE / streams ];
        count += vsearch( streams, remap, targetHash, s.size(), power );

        munmap( data, MMAP_SIZE );
    }

    return result;
}
