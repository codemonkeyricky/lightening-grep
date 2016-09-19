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

using namespace std;

#if 1
int kMod    = 0x800000;
int kBase   = 129;
#else
int kMod    = 909;
int kBase   = 26;
#endif

static int count = 0;
static int lineCount = 0;

#define MMAP_SIZE   ( 4096 * 4 )

int main()
{
    SearcherCPU     cpuSearch;
    SearcherAVX2    avx2Search;

    string file     = "big.txt2";
    string pattern  = "certificates";

//    cpuSearch.process( file, pattern );
    avx2Search.process( file, pattern );

//    cout << "Matches = " << matches.size() << endl;
//    cout << "Count = " << count << endl;
//    cout << "Line Count = " << lineCount << endl;

    return 0;
}
