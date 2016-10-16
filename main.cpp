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

using namespace std;


void patternFinder(
    vector< string >   * fileList,
    string              pattern
    )
{
    SearcherNativeAVX2  searcher;

    auto start = std::chrono::high_resolution_clock::now();

    vector< SearcherI::sFileSummary > ssv;

    string path;
    for ( auto & path : *fileList )
    {
        auto result = searcher.process( path, pattern );

        if ( result.size() > 0 )
            ssv.emplace_back( path, result );
    }

    auto finish = std::chrono::high_resolution_clock::now();

#if 0
    std::cout << "File Search took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;
    cout << "Total records processed = " << count << endl;
#endif

    Printer::FileSummary( ssv );
}


int main(
    int argc,
    char **argv
    )
{
    vector< string >    fileQ;
    string              dir( "." );

    if ( argc < 2 )
    {
        return -1;
    }

    string pattern = argv[ 1 ];

    string fileToSearch;
    if ( argc == 3 )
    {
        fileToSearch = argv[ 2 ];
    }

//    cout << pattern << endl;

    if ( fileToSearch == "" )
    {
        auto start = std::chrono::high_resolution_clock::now();

        cFileFinder         ff( fileQ, dir );
        ff.start();

        auto finish = std::chrono::high_resolution_clock::now();
        std::cout << "Directory traverse took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;
        cout << "Total records = " << fileQ.size() << endl;
    }
    else
    {
        fileQ.push_back( fileToSearch );
    }

    int workerThreads = 4;
    vector< thread >            pool;
    vector< vector< string > >  jobs;
    int jobLen = fileQ.size() / workerThreads;
    for ( auto i = 0; i < workerThreads; i ++ )
    {
        auto start = fileQ.begin() + jobLen * i;
        auto end = ( i != workerThreads - 1 ) ?
            ( fileQ.begin() + jobLen * ( i + 1 ) ) :
            fileQ.end();

        jobs.emplace_back( start, end );

        pool.push_back( thread( patternFinder, &jobs.back(), pattern ) );
    }

    for ( auto & thread : pool )
    {
        thread.join();
    }

    return 0;
}
