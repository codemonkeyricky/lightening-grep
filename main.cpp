#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <immintrin.h>
#include <assert.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

#include "cFileFinder.hpp"
#include "cGrep.hpp"
#include "cPrinter.hpp"
#include "cSearcherNative.hpp"

using namespace std;

int main(
    int argc,
    char **argv
    )
{
    if ( argc < 2 )
    {
        return -1;
    }

    std::vector< std::string > filters; 
    string filter;

    int index = 1;
    string pattern;
    string path;
    while ( index < argc )
    {
        // Parse options.
        if ( strncmp( argv[ index ], "-", 1 ) == 0 )
        {
            // Filter options.
            if ( strncmp( argv[ index ], "--", 2 ) == 0 )
            {
                filter = string( argv[ index ] + 2 );

                filters.emplace_back( filter );
            }
        }
        else
        if ( pattern == "" )
        {
            pattern = argv[ index ];
        }
        else
        {
            if ( path == "" )
            {
                path = argv[ index ];
            }
            else
            {
                return -1;
            }
        }

        index ++;
    }

    cGrep grepper( path, pattern, filters );
    grepper.start();

    return 0;
}
