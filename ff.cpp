#include <string>
#include <algorithm> 
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>

int LevenshteinDistance(
    std::string A, 
    std::string B
    )
{
    std::vector< std::vector< int > > mat( A.size() + 1, std::vector< int >( B.size() + 1 ) ); 

    for ( auto j = 0; j <= B.size(); j++ ) 
        mat[ 0 ][ j ] = j; 

    for ( auto j = 0; j <= A.size(); j++ ) 
        mat[ j ][ 0 ] = j; 

    for ( auto i = 1; i <= A.size(); i++ )
    {
        for ( auto j = 1; j <= B.size(); j++ )
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


#if 0
void dump(
    boost::filesystem::path path
    )
{
#if 0
    try
    {
        std::cout << (boost::filesystem::is_directory(path) ? 'D' : ' ') << ' ';
        std::cout << (boost::filesystem::is_symlink(path) ? 'L' : ' ') << ' ';
        for(int i = 0; i < level; ++i)
            std::cout << ' ';
#endif
        path.relative_path();
        // std::cout << path.relative_path() << std::endl;
        // std::cout << path.filename() << std::endl;
#if 0
    }
    catch(boost::filesystem::filesystem_error& fex)
    {
        std::cout << fex.what() << std::endl;
    }
#endif
}
#endif


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
