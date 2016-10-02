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

//#include "SearcherCPU.hpp"
//#include "SearcherAVX2.hpp"
#include "SearcherBFAVX2.hpp"
#include "cFileFinder.hpp"

using namespace std;

void patternFinder(
    cQueue< string >   * fileList,
    string              pattern
    )
{
    SearcherBFAVX2  searcher;

    auto start = std::chrono::high_resolution_clock::now();

    string path;
    while ( fileList->pop( path ) )
    {
        searcher.process( path, pattern );
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "File Search took "
        << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
        << " microseconds\n";
}


int main()
{
//    SearcherAVX2    avx2Search;

    cQueue< string > fileListQ;
    string p( "." );

    cFileFinder     ff( fileListQ, p );

    auto start = std::chrono::high_resolution_clock::now();

    ff.start();

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Directory traverse() took "
        << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
        << " microseconds\n";


//    cout << "### to print" << endl;
//    string path;
//    while ( fileListQ.pop( path ) )
//    {
//        cout << path << endl;
//    }
//    cout << "### " << endl;

    string file     = "big2.txt";
    string pattern  = "code.monkey.ricky";

//    string file     = "cQueueLockless.cpp";
//    string pattern  = "memory_order_relaxed";

    thread t1( patternFinder, &fileListQ, pattern );

//    start = std::chrono::high_resolution_clock::now();

//    cpuSearch.process( file, pattern );
//    auto matches = searcher.process( file, pattern );

//    finish = std::chrono::high_resolution_clock::now();
//    std::cout << "File Search took "
//        << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
//        << " microseconds\n";


//    cout << "Matches = " << matches.size() << endl;
//
//    for ( auto & instance : matches )
//    {
//        cout << "Line = " << instance.line << endl;
//    }
//    cout << "Line Count = " << lineCount << endl;

    t1.join();

    return 0;
}
