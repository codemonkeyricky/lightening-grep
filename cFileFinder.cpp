#include <dirent.h>
#include <string.h>

#include <iostream>

#include "cFileFinder.hpp"

using namespace std;

void cFileFinder::exploreDirectory(
    int                     workerThreads,
    std::string             root,
    iQueue< sSearchEntry >  *list
    )
{
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

                if ( strcmp( ext, ".c" ) == 0
                    || strcmp( ext, ".h" ) == 0
                    //                        || strcmp( ext, ".hpp" ) == 0
                    //                        || strcmp( ext, ".cpp" ) == 0
                    //                        || strcmp( ext, ".txt" ) == 0
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

                list->push( se );

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

        list->push( se );
    }
}

