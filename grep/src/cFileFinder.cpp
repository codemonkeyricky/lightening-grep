#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <unordered_set>
#include <cassert>
#if STD_FILESYSTEM
#include <experimental/filesystem>
#endif

#include "cFileFinder.hpp"

using namespace std;

std::unordered_set< std::string > f_allKnownTypes =
{
#include "cFileFinderTypeList.hpp"
};

std::map< std::string, std::unordered_set< std::string > > f_fileFilter =
{
#include "cFileFinderFilterList.hpp"
};


bool isBinary( std::string & name )
{
    int fd          = open( name.c_str(), O_RDONLY );
    int size        = lseek( fd, 0, SEEK_END );

    // Symbolic link will return size of -1;
    if ( size < 0 )
    {
//        std::cout << "### symblink : " << name << std::endl;

        return true;
    }

    lseek( fd, 0, SEEK_SET );
    char buf[ 128 ];

    int bufLen = size < sizeof( buf ) ? size : sizeof( buf );
    int len = read( fd, buf, bufLen );
    assert( bufLen == len );

    close( fd );

    char *curr = buf; 
    char *end = buf + bufLen;
    while ( curr != end && *( curr++ ) != 0 )
    { }

    bool result = ( curr != end );

    if ( result )
    {
//        std::cout << "### binary file : " << name << std::endl;
    }

    return result;
}


void cFileFinder::exploreDirectory(
    int                         workerThreads,
    std::string                 root,
    std::vector< std::string > &filters,
    iQueue< sGrepEntry >     &list
    )
{
    bool scanAllfiles = ( filters.size() == 0 ) ? true : false;

    std::unordered_set< std::string > filter; 
    if ( filters.size() > 0 )
    {
        for ( auto & f : filters ) 
        {
            auto type = f_fileFilter[ f ]; 
            filter.insert( type.begin(), type.end() );
        }
    }
    else
    {
        filter = f_allKnownTypes;
    }

    std::queue< std::string >   toExplore;

    toExplore.push( root );

    int count = 0;

    auto start = std::chrono::high_resolution_clock::now();
	
#if !STD_FILESYSTEM
	
	while ( toExplore.size() > 0 )
    {
        auto to_explore = toExplore.front();
        toExplore.pop();

        auto dirp = opendir( to_explore.c_str() );

        struct dirent * entry;
        while ( ( entry = readdir( dirp ) ) != nullptr )
        {
            auto name = entry->d_name;
            auto name_len = entry->d_reclen;

            if ( entry->d_type == DT_DIR )
            {
                if ( strcmp( name, "." ) == 0
                    || strcmp( name, ".." ) == 0 )
                {
                    continue;
                }

                if ( strncmp( name, ".", 1 ) == 0 )
                {
                    // Ignore all hidden directories.

                    continue;
                }

                string to_add;
                if ( to_explore != "." )
                {
                    to_add += to_explore + "/";
                }

                to_add += string( name );

                toExplore.push( to_add );

//                cout << "### path : " << to_add << endl;
            }
            else
            {
                if ( strncmp( name, ".", 1 ) == 0 )
                {
                    // Ignore all hidden files.

//                    continue;
                }

                int allow = 0;

                const char *curr = name;
                const char *ext = nullptr;
                while ( curr = strstr( curr, "." ) )
                {
                    ext = curr;
                    curr ++;
                }

#if 0
                if ( ext == nullptr )
                {
                    continue;
                }
#endif

                string to_add;
                if ( to_explore != "." )
                {
                    to_add += to_explore + "/";
                }

                to_add += string( name );

                if (   ( ext != nullptr && filter.find( ext ) != filter.end() ) 
                    || filter.find( name ) != filter.end ()
                )
                {
                    allow = 1;
                }
                else
                {
                    if ( scanAllfiles )
                    {
                        allow = !isBinary( to_add );
                        if ( allow )
                        {
                            // Skip ctags file.
                            allow = strcmp( name, "tags" ) != 0; 
                        }
                    }
                }

                if ( !allow )
                {
//                    cout << "to not add  ## " << name << endl;

                    continue;
                }

                sGrepEntry se( sGrepEntry::Msg::Search, to_add );

                list.push( se );

//                cout << to_add << endl;

                count ++;
            }
        }

        closedir( dirp );
    }
	
#else

    std::string path( "." );
    for ( auto & p : std::experimental::filesystem::recursive_directory_iterator( path ) )
    {
//        std::cout << p << std::endl;

        auto fullpath = p.path().string();

        auto copy = fullpath;
        auto lastIndex = fullpath.find_last_of( "/" );
        auto filename = fullpath.substr( lastIndex + 1 );

        lastIndex = filename.find_last_of( "." );
        std::string fileExtension;
        if ( lastIndex != std::string::npos )
        {
            fileExtension = filename.substr( lastIndex + 1 );
        }

        int allow = 0;
        if (   ( fileExtension != "" && filter.find( fileExtension ) != filter.end() )
            || filter.find( filename ) != filter.end ()
        )
        {
            allow = 1;
        }
        else
        {
            if ( scanAllfiles )
            {
                allow = !isBinary( fullpath );
            }
        }

        if ( !allow )
        {
            continue;
        }

        sGrepEntry se( sGrepEntry::Msg::Search, fullpath );

        list.push( se );

        count ++;
    }
	
#endif

    auto finish = std::chrono::high_resolution_clock::now();

#if 0
    std::cout << "File Search took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;
    cout << "######  files to process " << count << endl;
#endif 

    for ( auto i = 0; i < workerThreads; i ++ )
    {
        string empty;
        sGrepEntry se( sGrepEntry::Msg::Done, empty );

        list.push( se );
    }
}

