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
#include "cGrepEngineNative.hpp"
#include "cPrinter.hpp"

using namespace std;

class cGrepSearchSummaryPrinter : public iGrepSearchSummary
{
public:
    cGrepSearchSummaryPrinter( std::string & pattern )
        : m_pattern( pattern )
    { }

    ~cGrepSearchSummaryPrinter() { }

    virtual void push( sGrepFileSummary & fs )
    {
        cPrinter::print( fs, m_pattern );
    }

    virtual bool pop( sGrepFileSummary & fs )
    { }

private:
    std::string m_pattern;
};



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

    cGrepSearchSummaryPrinter  ss( pattern );
    cGrep grepper( path, pattern, filters, &ss );
    grepper.start();

//    std::vector< std::thread >    pool;
//    pool.emplace_back( cPrinter::processor, std::ref( ss ), std::ref( pattern ) );
//
//    for ( auto & thread : pool )
//    {
//        thread.join();
//    }

    return 0;
}
