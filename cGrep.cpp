
#include <cpuid.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "cGrep.hpp"

#include "cFileFinder.hpp"
#include "cPatternFinder.hpp"
#include "cPrinter.hpp"
#include "cSearcherNative.hpp"

using namespace std;

bool cGrep::avx_support = 0;
bool cGrep::avx2_support = 0;

int g_done = 0;


cGrep::cGrep(
    std::string & filePath,
    std::string & pattern
    )
: m_filePath( filePath ),
  m_pattern( pattern )
{
    unsigned int eax, ebx, ecx, edx;

    avx_support     = __builtin_cpu_supports( "avx" );
    avx2_support    = 0; // __builtin_cpu_supports( "avx2" );
}


cGrep::~cGrep()
{

}

extern int g_done;

void cGrep::startProducer(
    vector< thread >   &pool
    )
{
    if ( m_filePath != "" )
    {
        fileQ.push( m_filePath );

        g_done = 1;

        return;
    }

#if 0 
    auto start = std::chrono::high_resolution_clock::now();
    auto finish = std::chrono::high_resolution_clock::now();
#endif

    std::string  dir( "." );
    pool.emplace_back( cFileFinder::exploreDirectory, dir, &fileQ );
}


void cGrep::startConsumer(
    vector< thread >   &pool
    )
{
    int cap;
    cap = avx_support ? static_cast< int >( cPatternFinder::Capability::AVX ): 0x0; 
    cap |= avx2_support ? static_cast< int >( cPatternFinder::Capability::AVX2 ): 0x0; 

    if ( pool.size() > 0 )
    {
        int workerThreads = 4;
        for ( auto i = 0; i < workerThreads; i ++ )
        {
            pool.emplace_back( cPatternFinder::findPattern, i, cap, &fileQ, m_pattern );
        }
    }
    else
    {
        cPatternFinder::findPattern( 0, cap, &fileQ, m_pattern );
    }
}


void cGrep::start()
{
    vector< thread >    pool;

    startProducer( pool );

    startConsumer( pool );

    for ( auto & thread : pool )
    {
        thread.join();
    }
}
