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

#include "SearcherCPU.hpp"
#include "SearcherAVX2.hpp"
#include "SearcherSSE.hpp"

using namespace std;

int main()
{
    SearcherSSE     sseSearch;
    SearcherAVX2    avx2Search;

    string file     = "big2.txt";
    string pattern  = "code.monkey.ricky";

//    cpuSearch.process( file, pattern );
    auto matches = sseSearch.process( file, pattern );

    cout << "Matches = " << matches << endl;
//    cout << "Count = " << count << endl;
//    cout << "Line Count = " << lineCount << endl;

    return 0;
}
