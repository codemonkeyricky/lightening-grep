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
    cQueue< string >   * fileList,
    string              pattern
    )
{
    SearcherBFAVX2  searcher;

    auto start = std::chrono::high_resolution_clock::now();


    int count = 0;

    vector< SearcherI::sFileSummary > ssv;

    string path;
    while ( fileList->pop( path ) )
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


int main()
{
    cQueue< string >    fileQ;
    string              dir( "." );

    cFileFinder         ff( fileQ, dir );

    auto start = std::chrono::high_resolution_clock::now();

    ff.start();

    auto finish = std::chrono::high_resolution_clock::now();
//    std::cout << "Directory traverse took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;
//    cout << "Total records = " << fileQ.size() << endl;

    string pattern  = "msc_mmap_open";
//    string pattern  = "late_initcall_sync";

    patternFinder( &fileQ, pattern );

//    thread t1( patternFinder, &fileQ, pattern );
//    thread t2( patternFinder, &fileQ, pattern );
//    thread t3( patternFinder, &fileQ, pattern );
//    thread t4( patternFinder, &fileQ, pattern );

//    t1.join();
//    t2.join();
//    t3.join();
//    t4.join();

    return 0;
}
