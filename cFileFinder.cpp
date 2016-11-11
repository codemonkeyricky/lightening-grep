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
#include <stack>

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

struct DirNode
{
    DirNode( struct dirent ** nl, int i, int total, std::string & p )
        : namelist( nl ), curr( i ), total( total ), path( p )
        { }

    struct dirent **namelist; 
    int             curr;
    int             total;
    std::string     path;
};


void cFileFinder::exploreDirectory(
    int                         workerThreads,
    std::string                 root,
    std::vector< std::string > &filters,
    iQueue< sSearchEntry >     &list
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

    int count = 0;
    auto start = std::chrono::high_resolution_clock::now();

    std::stack< DirNode >   toExplore;

    struct dirent **namelist;
    auto total = scandir( root.c_str(), &namelist, nullptr, alphasort );
    toExplore.emplace( namelist, 0, total, root );

    while ( toExplore.size() > 0 )
    {
        auto & dirs = toExplore.top();

        bool cleanup = true;

        while ( dirs.curr < dirs.total )
        {
            auto name_len   = dirs.namelist[ dirs.curr ]->d_reclen;
            auto name       = dirs.namelist[ dirs.curr ]->d_name;

            if ( dirs.namelist[ dirs.curr ]->d_type == DT_DIR )
            {
                if ( strcmp( name, "." ) == 0
                    || strcmp( name, ".." ) == 0 )
                {
                    goto next;
                }

                if ( strncmp( name, ".", 1 ) == 0 )
                {
                    // Ignore all hidden directories.

                    goto next;
                }

                string to_add;
                if ( dirs.path != "." )
                {
                    to_add += dirs.path + "/";
                }

                to_add += string( name );

                struct dirent **namelist;
                auto total = scandir( to_add.c_str(), &namelist, nullptr, alphasort );
                toExplore.emplace( namelist, 0, total, to_add );

                dirs.curr ++;

                // Edge case.
                cleanup = false;

                break;
            }
            else
            {
//                if ( strncmp( name, ".", 1 ) == 0 )
//                {
//                    // Ignore all hidden files.
//
////                    continue;
//                }

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
                if ( dirs.path != "." )
                {
                    to_add += dirs.path + "/";
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
                    }
                }

                if ( !allow )
                {
//                    cout << "to not add  ## " << name << endl;

                    dirs.curr ++;

                    continue;
                }

                sSearchEntry se( sSearchEntry::Msg::Search, to_add );

                list.push( se );

//                cout << to_add << endl;

                count ++;
            }

            next:

            dirs.curr ++;
        }

        if ( dirs.curr >= dirs.total && cleanup )
        {
            for ( int i = 0; i < dirs.total; i++ )
            {
                free( dirs.namelist[ i ] );
            }
            free( dirs.namelist );

            toExplore.pop();
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "File Search took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;
    cout << "######  files to process " << count << endl;

    for ( auto i = 0; i < workerThreads; i ++ )
    {
        string empty;
        sSearchEntry se( sSearchEntry::Msg::Done, empty );

        list.push( se );
    }
}

