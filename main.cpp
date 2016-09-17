#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <immintrin.h>
#include <assert.h>

using namespace std;

struct Match
{
    Match( int line, int pos ) :
        line( line ), pos( pos )
    { }

    int line;
    int pos;
};


#if 1
int kMod    = 0x800000;
int kBase   = 129;
#else
int kMod    = 909;
int kBase   = 26;
#endif

static int count = 0;

void vsearch(
    __m128i input[ 1024 ],
    __m128i output[ 1024 ],
    int targetHash,
    int slen,
    int p
    )
{
    int mapSize = 4096;
    int result[ 4 ] = { 0 }; 

    __m128i zeroes          = _mm_set_epi32( 0, 0, 0, 0 );
    __m128i ones            = _mm_set_epi32( 1, 1, 1, 1 );
    __m128i currHash        = _mm_set_epi32( 0, 0, 0, 0 );
    __m128i targetHash128   = _mm_set_epi32( targetHash, targetHash, targetHash, targetHash );
    __m128i modMask         = _mm_set_epi32( kMod - 1, kMod - 1, kMod - 1, kMod - 1 );

    __m128i power128        = _mm_set_epi32( p, p, p, p );
    __m128i kBase128        = _mm_set_epi32( kBase, kBase, kBase, kBase );
    __m128i kMod128         = _mm_set_epi32( kMod, kMod, kMod, kMod );
    __m128i rollingHash128  = zeroes;

    for ( auto i = 0; i < slen; i++ )
    {
        // thash = ( thash * kBase + t[ i ] ) % kMod;

        auto tadd       = input[ i ]; // _mm_load_si128( reinterpret_cast< __m128i *>( &input[ i * 4 ] ) );

        auto a          = _mm_mullo_epi32( rollingHash128, kBase128 );
        auto b          = _mm_add_epi32( a, tadd );

        rollingHash128  = _mm_and_si128( b, modMask );
    }

    vector< int > hashes[ 4 ];

    int index = 0; 
    for ( auto i = slen; i < mapSize / 4; i++ )
    {
        // if ( rollingHash == targetHash ) { }

        auto result = _mm_cmpeq_epi32( rollingHash128, targetHash128 );
        output[ i ] = result;

        // thash -= ( t[ i - s.size() ] * power ) % kMod;

        // printf( "%c", input[ 1 ] );
        auto remove         = input[ i - 5 ]; // _mm_load_si128( reinterpret_cast< __m128i *>( &input[ ( i - 5 ) * 4 ] ) );
        auto a              = _mm_mullo_epi32( remove, power128 );
        auto b              = _mm_and_si128( a, modMask );
        rollingHash128      = _mm_sub_epi32( rollingHash128, b );

        // if ( thash < 0 ) thash += kMod;

        auto cmp            = _mm_cmplt_epi32( rollingHash128, zeroes );
        auto to_add         = _mm_and_si128( kMod128, cmp );
        rollingHash128      = _mm_add_epi32( rollingHash128, to_add );

        // thash = ( thash * kBase + t[ i ] ) % kMod;

        auto tadd           = input[ i ]; // _mm_load_si128( reinterpret_cast< __m128i *>( &input[ i * 4 ] ) );

        a                   = _mm_mullo_epi32( rollingHash128, kBase128 );
        b                   = _mm_add_epi32( a, tadd );

        rollingHash128      = _mm_and_si128( b, modMask );
    }

//    vector< int > total_hashes;
//    for ( auto k = 0; k < 4; k++ )
//    {
//        total_hashes.insert( total_hashes.end(), hashes[ k ].begin(), hashes[ k ].end() );
//    }

    for ( auto i = 0; i < 4; i++ )
    {
        for ( auto & hash : hashes[ i ] )
        {
//            printf( "0x%08x\n", hash );
        }

//        printf( "new set\n" );
    }

//    return result[ 0 ]
//   + result [ 1 ]
//   + result [ 2 ]
//   + result [ 3 ]  ;
}


int main()
{
    int mapSize = 4096;
    int fd      = open( "big.txt", O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s    = "woman";

    vector< Match > matches;

    for ( auto k = 0; k < size; k += mapSize )
    {
        auto data = mmap( 0, 4096, PROT_READ, MAP_SHARED, fd, k );

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

//        printf( "target hash : 0x%08x\n", targetHash );

#if 0
        int found = 0;
        for ( auto i = s.size(); i < mapSize; i++ )
        {
            if ( rollingHash == targetHash )
            {
                char * val = &( t[ i - s.size() ] );

                if ( memcmp( s.c_str(), &t[ i - s.size() ], s.size() ) == 0 )
                {
                    matches.emplace_back( 0, k + i );
                }
            }

//            printf( "0x%08x\n", rollingHash );

            rollingHash -= ( t[ i - s.size() ] * power ) % kMod;
            if ( rollingHash < 0 )
            {
                rollingHash += kMod;
            }

            rollingHash = ( rollingHash * kBase + t[ i ] ) % kMod;
        }
#endif

#if 1
        __m128i remap[ mapSize ];
        for ( auto i = 0; i < mapSize / 4; i++ )
        {
            int *input = ( int * ) &remap[ i ];
            input[ 0 ] = t[ 1024 * 0 + i ];
            input[ 1 ] = t[ 1024 * 1 + i ];
            input[ 2 ] = t[ 1024 * 2 + i ];
            input[ 3 ] = t[ 1024 * 3 + i ];
        }
#endif

        char thash_arr[ 4 ] = { 0 };
//        int output[ mapSize ];
        __m128i output[ 1024 ];
        vsearch( remap, output, targetHash, s.size(), power );

//        auto temp = output[ 0 ];

        for ( auto i = 0; i < 1024; i++ )
        {
            count += ( ( int * ) &output[ i ] )[ 0 ] & 1;
            count += ( ( int * ) &output[ i ] )[ 1 ] & 1;
            count += ( ( int * ) &output[ i ] )[ 2 ] & 1;
            count += ( ( int * ) &output[ i ] )[ 3 ] & 1;
        }

//        printf( "%c", thash_arr[ 2 ] );
    }

    cout << "Matches = " << matches.size() << endl;
    cout << "Count = " << count << endl;

    return 0;
}
