
#include <cpuid.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "cGrep.hpp"

#include "cFileFinder.hpp"
#include "cGrepEngineNative.hpp"
#include "cPatternFinder.hpp"
#include "GrepCommon.hpp"

using namespace std;

bool cGrep::avx_support = 0;
bool cGrep::avx2_support = 0;

cGrep::cGrep(
    std::string                &filePath,
    std::string                &pattern,
    std::vector< std::string > &filters,
    iGrepSearchSummary         *ss
    )
: m_filePath( filePath ),
  m_pattern( pattern ),
  m_filters( filters ),
  m_summary( ss )
{
    avx_support     = __builtin_cpu_supports( "avx" );
    avx2_support    = __builtin_cpu_supports( "avx2" );
}


cGrep::~cGrep()
{

}

void cGrep::startProducer(
    vector< thread >        &pool,
    iQueue< sGrepEntry >  &list
    )
{
    if ( m_filePath != "" )
    {
        sGrepEntry se( sGrepEntry::Msg::Search, m_filePath );
        list.push( se );

        string empty;
        sGrepEntry done( sGrepEntry::Msg::Done, empty );
        list.push( done );

        return;
    }

#if 0 
    auto start = std::chrono::high_resolution_clock::now();
    auto finish = std::chrono::high_resolution_clock::now();
#endif

    std::string  dir( "." );
    pool.emplace_back( 
            cFileFinder::exploreDirectory, 
            m_workerThreads, 
            dir, 
            std::ref( m_filters ), 
            std::ref( list ) 
            );
}


void cGrep::startConsumer(
    vector< thread >        &pool,
    iQueue< sGrepEntry >  &list
    )
{
    int cap;
    cap = avx_support ? static_cast< int >( cPatternFinder::Capability::AVX ): 0x0; 
    cap |= avx2_support ? static_cast< int >( cPatternFinder::Capability::AVX2 ): 0x0; 

    if ( pool.size() > 0 )
    {
        for ( auto i = 0; i < m_workerThreads; i ++ )
        {
            pool.emplace_back( cPatternFinder::findPattern, i, cap, &list, m_pattern, m_summary );
        }
    }
    else
    {
        cPatternFinder::findPattern( 0, cap, &list, m_pattern, m_summary );
    }
}


void cGrep::start()
{
    vector< thread >        pool;
    cQueue< sGrepEntry >  list;

    startProducer( pool, list );

    startConsumer( pool, list );

    for ( auto & thread : pool )
    {
        thread.join();
    }
}
