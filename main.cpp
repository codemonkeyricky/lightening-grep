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
    char    t[ 4096 ],
    char    m[ 4096 ],
    int     shash,
    int     slen,
    int     p
    )
{
    int mapSize = 1024;
    int result[ 4 ] = { 0 }; 

    __m128i zeroes      = _mm_set_epi32( 0, 0, 0, 0 );
    __m128i ones        = _mm_set_epi32( 1, 1, 1, 1 );
    __m128i currHash    = _mm_set_epi32( 0, 0, 0, 0 );
    __m128i targetHash  = _mm_set_epi32( shash, shash, shash, shash );
    __m128i modMask     = _mm_set_epi32( kMod - 1, kMod - 1, kMod - 1, kMod - 1 );

//    __m128i remove;
    __m128i power       = _mm_set_epi32( p, p, p, p );
    __m128i base        = _mm_set_epi32( kBase, kBase, kBase, kBase );
    __m128i mod         = _mm_set_epi32( kMod, kMod, kMod, kMod );
    volatile __m128i thash;

    __m128i remove[ slen ];

    int output[ 4 ];
    for ( auto i = 0; i < slen; i++ )
    {
        // thash = ( thash * kBase + t[ i ] ) % kMod;

        int input[ 4 ] = { t[ i * 4 + 0 ], t[ i * 4 + 1 ], t[ i * 4 + 2 ], t[ i * 4 + 3 ] };

        auto tadd   = _mm_load_si128( reinterpret_cast< __m128i *>( &input[ 0 ] ) );
        remove[ i ] = tadd;

        auto a      = _mm_mullo_epi32( thash, base );
        auto b      = _mm_add_epi32( a, tadd );

        thash       = _mm_and_si128( b, modMask );

        _mm_store_si128( reinterpret_cast< __m128i *>( &output[ 0 ] ), thash );
    }

    for ( auto i = slen; i < mapSize / 4; i++ )
    {
        // if ( rollingHash == targetHash ) { }
        int output[ 4 ];
        auto result = _mm_cmpeq_epi32( thash, targetHash );
        _mm_store_si128( reinterpret_cast< __m128i *>( output ), result );
        m[ i - slen + 0 ] = ( ( char * ) output )[ 0 ];
        m[ i - slen + 1 ] = ( ( char * ) output )[ 4 ];
        m[ i - slen + 2 ] = ( ( char * ) output )[ 8 ];
        m[ i - slen + 3 ] = ( ( char * ) output )[ 12 ];

//        // TODO:
//        if ( m[ i - slen + 0 ]
//        || m[ i - slen + 1 ]
//        || m[ i - slen + 2 ]
//        || m[ i - slen + 3 ]
//        )
//        {
//            count++;
//        }

        int input[ 4 ] = { t[ i * 4 + 0 ], t[ i * 4 + 1 ], t[ i * 4 + 2 ], t[ i * 4 + 3 ] };

        // thash -= ( t[ i - s.size() ] * power ) % kMod;

        auto a      = _mm_mul_epu32( remove[ ( i - slen ) % slen ], power );
        auto b      = _mm_and_si128( a, modMask );
        thash       = _mm_sub_epi32( thash, b );

        // if ( thash < 0 ) thash += kMod;

        auto cmp    = _mm_cmplt_epi32( thash, zeroes );
        auto to_add = _mm_and_si128( mod, cmp );
        thash       = _mm_add_epi32( thash, to_add );

        // thash = ( thash * kBase + t[ i ] ) % kMod;

        auto tadd           = _mm_load_si128( reinterpret_cast< __m128i *>( &input[ 0 ] ) );
        remove[ i % slen ]  = tadd;

        a           = _mm_mul_epu32( thash, base );
        b           = _mm_add_epi32( a, tadd );

        thash       = _mm_and_si128( b, modMask );
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
    string s    = "EBook";

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

            rollingHash -= ( t[ i - s.size() ] * power ) % kMod;
            if ( rollingHash < 0 )
            {
                rollingHash += kMod;
            }

            rollingHash = ( rollingHash * kBase + t[ i ] ) % kMod;
        }
#endif

#if 1
        char remap[ mapSize ];
        for ( auto i = 0; i < mapSize / 4; i++ )
        {
            remap[ i * 4 + 0 ] = t[ 1024 * 0 + i ];
            remap[ i * 4 + 1 ] = t[ 1024 * 1 + i ];
            remap[ i * 4 + 2 ] = t[ 1024 * 2 + i ];
            remap[ i * 4 + 3 ] = t[ 1024 * 3 + i ];
        }
#endif

        char thash_arr[ 4 ] = { 0 };
        char output[ mapSize ];
        vsearch( remap, output, targetHash, s.size(), power );

        printf( "%c", thash_arr[ 2 ] );
    }

    cout << "Matches = " << matches.size() << endl;
    cout << "Count = " << count << endl;

    return 0;
}
