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

#include "SearcherNativeAVX2.hpp"
#include "SearcherRabinKarpAVX2.hpp"
#include "cFileFinder.hpp"
#include "Printer.hpp"
#include "cGrep.hpp"

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

    string pattern;
    if ( argc >= 2 )
    {
        pattern = argv[ 1 ];
    }

    string path;
    if ( argc >= 3 )
    {
        path = argv[ 2 ];
    }

    cGrep grepper( path, pattern );
    grepper.start();

    return 0;
}
