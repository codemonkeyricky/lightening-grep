#include <dirent.h>
#include <string.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <unordered_set>

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

    std::queue< std::string >   toExplore;

    toExplore.push( root );

    int count = 0;

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

                    continue;
                }

                int allow = 0;

                const char *curr = name;
                const char *ext = nullptr;
                while ( curr = strstr( curr, "." ) )
                {
                    ext = curr;
                    curr ++;
                }

                if ( ext == nullptr )
                {
                    continue;
                }

                if (   filter.find( ext ) != filter.end()
                    || filter.find( name ) != filter.end ()
                )
                {
                    allow = 1;
                }

                if ( !allow )
                {
//                    cout << "to not add  ## " << name << endl;

                    continue;
                }

                string to_add;
                if ( to_explore != "." )
                {
                    to_add += to_explore + "/";
                }

                to_add += string( name );

                sSearchEntry se( sSearchEntry::Msg::Search, to_add );

                list.push( se );

//                cout << to_add << endl;

                count ++;
            }
        }

        closedir( dirp );
    }

    cout << "######  files to process " << count << endl;

    for ( auto i = 0; i < workerThreads; i ++ )
    {
        string empty;
        sSearchEntry se( sSearchEntry::Msg::Done, empty );

        list.push( se );
    }
}

