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

#include "SearcherRabinKarpAVX2.hpp"
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

    int index = 1;
    string filter;

    if ( strncmp( argv[ index ], "--", 2 ) == 0 )
    {
        filter = string( argv[ index ] + 2 );

        index++;
    }

    string pattern;
    if ( ( argc - index ) > 0)
    {
        pattern = argv[ index ];

        index++;
    }

    string path;
    if ( ( argc - index ) > 0)
    {
        path = argv[ index ];
    }

    std::vector< std::string > filters = { filter }; 

    cGrep grepper( path, pattern, filters );
    grepper.start();

    return 0;
}
