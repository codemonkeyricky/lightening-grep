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

//#include "SearcherCPU.hpp"
//#include "SearcherAVX2.hpp"
#include "SearcherBFAVX2.hpp"
#include "cFileFinder.hpp"

using namespace std;

int main()
{
    SearcherBFAVX2  searcher;
//    SearcherAVX2    avx2Search;

    cQueue< string > q;
    string p( "." );

    cFileFinder     ff( q, p );

    ff.start();

    cout << "### to print" << endl;
    string path;
    while ( q.pop( path ) )
    {
        cout << path << endl;
    }
    cout << "### " << endl;

    string file     = "big2.txt";
    string pattern  = "code.monkey.ricky";

//    string file     = "cQueueLockless.cpp";
//    string pattern  = "memory_order_relaxed";

//    cpuSearch.process( file, pattern );
    auto matches = searcher.process( file, pattern );

    cout << "Matches = " << matches.size() << endl;

    for ( auto & instance : matches )
    {
        cout << "Line = " << instance.line << endl;
    }
//    cout << "Line Count = " << lineCount << endl;

    return 0;
}
