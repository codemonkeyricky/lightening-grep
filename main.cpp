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

#include "SearcherBFAVX2.hpp"
#include "SearcherRabinKarpAVX2.hpp"
#include "cFileFinder.hpp"
#include "Printer.hpp"

using namespace std;


void patternFinder(
    vector< string >   * fileList,
    string              pattern
    )
{
    SearcherBFAVX2  searcher;

    auto start = std::chrono::high_resolution_clock::now();


    int count = 0;

    vector< SearcherI::sFileSummary > ssv;

    string path;
//    while ( fileList->pop( path ) )
    for ( auto & path : *fileList )
    {
//        string test( "include/linux/init.h" );
//        if ( !path.compare( test ) )
//        {
////            assert( 1 );
//            count++;
//        }

        auto result = searcher.process( path, pattern );

        if ( result.size() > 0 )
            ssv.emplace_back( path, result );
    }

    auto finish = std::chrono::high_resolution_clock::now();
//    std::cout << "File Search took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;

//    cout << "Total records processed = " << count << endl;

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

//    string pattern  = "msc_mmap_open";
//    string pattern  = "late_initcall_sync";

#if 0
    patternFinder( &fileQ, pattern );
#else

    auto divider = 4;
    std::vector< string > q1( fileQ.begin() + fileQ.size() / divider * 0, fileQ.begin() + fileQ.size() / divider * 1 );
    std::vector< string > q2( fileQ.begin() + fileQ.size() / divider * 1, fileQ.begin() + fileQ.size() / divider * 2 );
    std::vector< string > q3( fileQ.begin() + fileQ.size() / divider * 2, fileQ.begin() + fileQ.size() / divider * 3 );
    std::vector< string > q4( fileQ.begin() + fileQ.size() / divider * 3, fileQ.begin() + fileQ.size() / divider * 4 );
//    std::vector< string > q5( fileQ.begin() + fileQ.size() / divider * 4, fileQ.begin() + fileQ.size() / divider * 5 );

    thread t1( patternFinder, &q1, pattern );
    thread t2( patternFinder, &q2, pattern );
    thread t3( patternFinder, &q3, pattern );
    thread t4( patternFinder, &q4, pattern );
//    thread t5( patternFinder, &q5, pattern );

    t1.join();
    t2.join();
    t3.join();
    t4.join();
//    t5.join();
#endif

    return 0;
}
