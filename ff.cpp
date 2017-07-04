#include <string>
#include <algorithm> 
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
#include <cstring>
#include <chrono>


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
#define MAX_CHAR        32
#define CHAR_PER_REG    16

    std::vector< uint8_t > initial = 
    { 
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    }; 

    std::vector< uint8_t > t = initial;
    t.resize( MAX_CHAR + 1 ); 
    std::vector< uint8_t > ts; 
    std::copy( t.begin() + 1, t.end(), std::back_inserter( ts ) ); 
    ts.resize( MAX_CHAR + 1 ); 

    auto targetLen = target.length();
    auto candidateLen = candidate.length();
    auto copy = target; 
    copy.resize( MAX_CHAR + 1 ); 

    // Calculate number of xmm registers required.
    auto regReq = targetLen / CHAR_PER_REG;
    regReq += targetLen % CHAR_PER_REG ? 1 : 0;

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
            c[ j ]  = _mm_load_si128( ( const __m128i * ) &t[ j * CHAR_PER_REG ] );
            cs[ j ] = _mm_load_si128( ( const __m128i * ) &ts[ j * CHAR_PER_REG ] );
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
            sum         = _mm_slli_si128( sum, 1 );

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

        ts.clear();
        std::copy( t.begin() + 1, t.end(), std::back_inserter( ts ) ); 
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

#if 0
    std::priority_queue< Element, std::vector< Element >, Compare > heap; 
    for ( auto &p : list ) 
    {
#endif 
#if 0
        std::string t( "source.c" ); 
        std::string p( "sourc" ); 
        #endif
        std::string t( "bread" ); 
        std::string p( "fred" ); 

        auto t1 = std::chrono::high_resolution_clock::now(); 

        for ( auto i = 0; i < 1; i ++ )
        {
            volatile int d1 = LevenshteinDistance( t, p );
            volatile int d0 = 0;
        }

        auto t2 = std::chrono::high_resolution_clock::now(); 

        for ( auto i = 0; i < 1; i ++ )
        {
            volatile int d2 = LevenshteinDistanceScalar( t, p ); 
            volatile int d0 = 0;
        }

        auto t3 = std::chrono::high_resolution_clock::now(); 

        std::cout << "SIMD:     " << std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count() << " us" << std::endl; 
        std::cout << "SCALAR:   " << std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count() << " us" << std::endl; 

#if 0
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
#endif
}
