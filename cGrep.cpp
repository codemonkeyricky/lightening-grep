
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
#include "sSearchCommon.hpp"

using namespace std;

bool cGrep::avx_support = 0;
bool cGrep::avx2_support = 0;

cGrep::cGrep(
    std::string & filePath,
    std::string & pattern
    )
: m_filePath( filePath ),
  m_pattern( pattern )
{
    unsigned int eax, ebx, ecx, edx;

    avx_support     = __builtin_cpu_supports( "avx" );
    avx2_support    = __builtin_cpu_supports( "avx2" );
}


cGrep::~cGrep()
{

}

void cGrep::startProducer(
    vector< thread >        &pool,
    iQueue< sSearchEntry >  &list
    )
{
    if ( m_filePath != "" )
    {
        sSearchEntry se( sSearchEntry::Msg::Search, m_filePath );
        list.push( se );

        string empty;
        sSearchEntry done( sSearchEntry::Msg::Done, empty );
        list.push( done );

        return;
    }

#if 0 
    auto start = std::chrono::high_resolution_clock::now();
    auto finish = std::chrono::high_resolution_clock::now();
#endif

    std::string  dir( "." );
    pool.emplace_back( cFileFinder::exploreDirectory, m_workerThreads, dir, &list );
}


void cGrep::startConsumer(
    vector< thread >        &pool,
    iQueue< sSearchEntry >  &list
    )
{
    int cap;
    cap = avx_support ? static_cast< int >( cPatternFinder::Capability::AVX ): 0x0; 
    cap |= avx2_support ? static_cast< int >( cPatternFinder::Capability::AVX2 ): 0x0; 

    if ( pool.size() > 0 )
    {
        for ( auto i = 0; i < m_workerThreads; i ++ )
        {
            pool.emplace_back( cPatternFinder::findPattern, i, cap, &list, m_pattern );
        }
    }
    else
    {
        cPatternFinder::findPattern( 0, cap, &list, m_pattern );
    }
}


void cGrep::start()
{
    vector< thread >        pool;
    cQueue< sSearchEntry >  list;

    startProducer( pool, list );

    startConsumer( pool, list );

    for ( auto & thread : pool )
    {
        thread.join();
    }
}
