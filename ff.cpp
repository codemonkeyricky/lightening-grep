#include <string>
#include <algorithm> 
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
#include <cstring>
#include <chrono>
#include <cassert>


int LevenshteinDistanceScalar(
    std::string A,
    std::string B
    )
{
    std::vector< std::vector< int > > mat( A.size() + 1, std::vector< int >( B.size() + 1 ) ); 

    for ( auto j = 0u; j <= B.size(); j++ ) 
        mat[ 0 ][ j ] = j; 

    for ( auto j = 0u; j <= A.size(); j++ ) 
        mat[ j ][ 0 ] = j; 

    for ( auto i = 1u; i <= A.size(); i++ )
    {
        for ( auto j = 1u; j <= B.size(); j++ )
        {
            if ( A[ i - 1 ] == B[ j - 1 ] ) 
            {
                mat[ i ][ j ] = mat[ i - 1 ][ j - 1 ];
            }
            else
            {
                mat[ i ][ j ] = std::min( std::min( mat[ i - 1 ][ j - 1 ], mat[ i ][ j - 1 ] ), 
                        mat[ i - 1 ][ j ] ); 

                mat[ i ][ j ] += 1; 
            }
        }
    }

    return mat[ A.size() ][ B.size() ]; 
}



int LevenshteinDistance(
    std::string target, 
    std::string candidate
    )
{
#define MAX_CHAR        64
#define CHAR_PER_REG    16

    if ( candidate.length() >= MAX_CHAR )
    {
        std::cout << candidate << std::endl; 
    }

    assert( target.length() <= ( 64 - 1 ) );
    assert( candidate.length() <= ( 64 - 1 ) );

    uint8_t initial[ 64 + 1 ] = 
    { 
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        0xff 
    }; 

    uint8_t t[ MAX_CHAR ]; 
    uint8_t ts[ MAX_CHAR ]; 
    memcpy( t, initial, MAX_CHAR ); 
    memcpy( ts, initial + 1, MAX_CHAR ); 

    auto targetLen      = target.length();
    auto candidateLen   = candidate.length();
    uint8_t copy[ MAX_CHAR ] = { 0 };
    memcpy( &copy[ 0 ], &target[ 0 ], target.length() );

    // Calculate number of xmm registers required.
    auto regReq = ( targetLen + 1 ) / CHAR_PER_REG;
    regReq += ( targetLen + 1 ) % CHAR_PER_REG ? 1 : 0;

    // Initialize current and current shifted.
    __m128i c[ regReq ], cs[ regReq ];
    for ( auto i = 0u; i < regReq; i++ )
    {
        c[ i ]  = _mm_load_si128( ( const __m128i * ) &( t[ i * CHAR_PER_REG ] ) );
    }

    // Initialize ones. 
    auto o = _mm_set1_epi8( 1 ); 

    // Initialize source variable.
    __m128i s[ regReq ];
    for ( auto i = 0u; i < regReq; i++ )
    {
        s[ i ] = _mm_load_si128( ( const __m128i * ) &( copy[ i * CHAR_PER_REG ] ) ); 
    }

    // Create edit and add arrays.
    __m128i e[ regReq ], a[ regReq ]; 

    for ( auto i = 0u; i < candidateLen; i ++ )
    {
        // 
        // Prepare the shifted variable for next iteration.
        // 

        for ( auto j = 0u; j < regReq; j++ )
        {
            uint8_t temp[ CHAR_PER_REG ]; 

            // Any multiple of 32 chars needs to be offset. 

            uint8_t o = ( j == 0 ) ? 0 : 1; 

            memcpy( temp, &t[ j * CHAR_PER_REG - o ], CHAR_PER_REG ); 
            c[ j ]  = _mm_load_si128( ( const __m128i * ) &temp[ 0 ] ); 

            memcpy( temp, &ts[ j * CHAR_PER_REG - o ], CHAR_PER_REG ); 
            cs[ j ]  = _mm_load_si128( ( const __m128i * ) &temp[ 0 ] ); 
        }

        for ( auto j = 0u; j < regReq; j++ )
        {
            // edit just mirrorss current.
            e[ j ]  = c[ j ]; 

            // add needs to be shifted. 
            a[ j ]  = cs[ j ];

            // Only need to substitute if character is different.
            auto r  = _mm_set1_epi8( candidate[ i ] );

            // m = compare entire word with current letter
            // e = m * e + !m * ( min( a, e ) + 1 ); 

            // 
            // Find the chars that just carries over -> p1
            // 

            auto m  = _mm_cmpeq_epi8( s[ j ], r );
            auto p1 = _mm_and_si128( m, e[ j ] ); 

            // 
            // Find the minimum between edits and adds, then +1 -> p2 
            // 

            auto min    = _mm_min_epi8( a[ j ], e[ j ] ); 
            min         = _mm_add_epi8( min, o );
            auto nm     = ~m; 
            auto p2     = _mm_and_si128( nm, min ); 

            auto sum    = _mm_add_epi8( p1, p2 ); 

            if ( j == 0 ) 
                sum = _mm_slli_si128( sum, 1 );

            _mm_store_si128( ( __m128i * ) &t[ j * CHAR_PER_REG ], sum ); 
        }

        // 
        // Now account for deletes -> c 
        // 

        t[ 0 ] = i + 1; 
        for ( auto j = 1u; j < ( targetLen + 1 ); j ++ )
        {
            t[ j ] = std::min( (uint8_t) ( t[ j - 1 ] + 1 ), t[ j ] ); 
        }

        memcpy( ts, t + 1, MAX_CHAR ); 
    }

    return t[ targetLen ]; 
}



struct FileEntry
{
    FileEntry( std::string f, std::string p ) 
        : f( f ), p( p )
    { }

    std::string f;  ///< filename 
    std::string p;  ///< fullpath
};


struct Element
{
    Element( int d, const std::string f, const std::string & p ) 
        : d( d ), f( f ), p( p )
    { }

    int d;          ///< distance
    std::string f;  ///< filename
    std::string p;  ///< path
}; 


struct Compare
{
    bool operator() (
        const Element   &e1,
        const Element   &e2
        )
    {
        return e1.d < e2.d; 
    }
};


int main(
    int argc, 
    char **argv
    )
{
    std::string pattern = argv[ 1 ]; 

    std::vector< FileEntry > list;
    std::fstream f( "filelist" );
    std::string fullpath;
    while ( true )
    {
        std::getline( f, fullpath ); 
        if ( !f )
            break; 

        auto lastIndex  = fullpath.find_last_of( "/" );
        auto filename   = fullpath.substr( lastIndex + 1 );

        list.emplace_back( filename, fullpath ); 
    }

    std::priority_queue< Element, std::vector< Element >, Compare > heap; 
    for ( auto &p : list ) 
    {
        auto d = LevenshteinDistance( pattern, p.f );

        if ( heap.size() < 10 )
        {
            heap.emplace( d, p.f, p.p );
        }
        else if ( d < heap.top().d )
        {
            heap.pop(); 
            heap.emplace( d, p.f, p.p ); 
        }
    }

    while ( heap.size() > 0 )
    {
        std::cout << heap.top().d << " : " << heap.top().p << std::endl; heap.pop();
    }
}
