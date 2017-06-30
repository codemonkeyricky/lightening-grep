
#include <boost/filesystem.hpp>


void dump(boost::filesystem::path path, int level)
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


int main()
{
    std::string path( "." );

    auto it = boost::filesystem::recursive_directory_iterator( path ); 
    boost::filesystem::recursive_directory_iterator end; 
    for ( ; it != end; it ++ ) 
    {
        dump( *it, 0 ); 
    }
}
