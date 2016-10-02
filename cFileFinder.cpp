#include <dirent.h>
#include <string.h>

#include <iostream>

#include "cFileFinder.hpp"

using namespace std;

cFileFinder::cFileFinder(
    cQueue< std::string >   &q,
    std::string             &p
    )
: m_fileList( q ),
  m_rootPath( p )
{
}


cFileFinder::~cFileFinder()
{

}


void cFileFinder::start()
{
    m_pathToExplore.push( m_rootPath );

    while ( m_pathToExplore.size() > 0 )
    {
        auto to_explore = m_pathToExplore.front();
        m_pathToExplore.pop();

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

                m_pathToExplore.push( to_add );

//                cout << "### path : " << to_add << endl;
            }
            else
            {
                if ( strncmp( name, ".", 1 ) == 0 )
                {
                    // Ignore all hidden files.

                    continue;
                }

                auto ext = strstr( name, "." );
                if ( ext == nullptr )
                {
                    continue;
                }

                int allow = 0;
                if ( strcmp( ext, ".cpp" ) == 0
                    || strcmp( ext, ".hpp" ) == 0
                )
                {
                    allow = 1;
                }

                if ( !allow )
                {
                    continue;
                }

                string to_add;
                if ( to_explore != "." )
                {
                    to_add += to_explore + "/";
                }

                to_add += string( name );

//                cout << "### file : " << to_add << endl;

                m_fileList.push( to_add );
            }
        }

        closedir( dirp );
    }
}
