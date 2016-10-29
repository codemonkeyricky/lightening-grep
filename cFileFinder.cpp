#include <dirent.h>
#include <string.h>

#include <iostream>

#include "cFileFinder.hpp"

using namespace std;

extern int g_done;

void cFileFinder::exploreDirectory(
    std::string             root,
    iQueue< std::string >  *list
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

                int allow = 1;

                auto ext = strstr( name, "." );
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

                list->push( to_add );


                count ++;
            }
        }

        closedir( dirp );
    }

    cout << "######  files to process " << count << endl;

    g_done = 1;
}

