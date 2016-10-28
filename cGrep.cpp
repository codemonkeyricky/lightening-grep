
#include <cpuid.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "cGrep.hpp"

#include "cFileFinder.hpp"
#include "cPrinter.hpp"
#include "cSearcherNative.hpp"

using namespace std;

bool cGrep::avx_support = 0;
bool cGrep::avx2_support = 0;

int g_done = 0;

void cGrep::patternFinder(
    int                 workerId,
    cQueue< string >   *fileList,
    string              pattern
    )
{
    iSearcher  *searcher;
    cSearcherNative< AVX >  savx( pattern );
    cSearcherNative< AVX2 >  savx2( pattern );

    searcher = ( avx2_support ) ?
        static_cast< iSearcher * >( &savx2 ) :
        static_cast< iSearcher * >( &savx );

    auto start = std::chrono::high_resolution_clock::now();

    vector< iSearcher::sFileSummary > ssv;

    while ( !g_done || fileList->size() > 0 )
    {
        string path;
        while ( fileList->pop( path ) )
        {
            auto result = searcher->process( path );

            if ( result.size() > 0 )
            {
                ssv.emplace_back( path, result );
            }
        }

        usleep( 1 );
    }

    auto finish = std::chrono::high_resolution_clock::now();

#if 0
    std::cout << "File Search took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;

    cout << "Worker ID = " << workerId << " " <<
        "Records processed = " << fileList->size() << " "
        "Match found = " << ssv.size() << endl;
#endif

    cPrinter::print( ssv );
}


cGrep::cGrep(
    std::string & filePath,
    std::string & pattern
    )
: m_filePath( filePath ),
  m_pattern( pattern )
{
    unsigned int eax, ebx, ecx, edx;

//    __get_cpuid( 1, &eax, &ebx, &ecx, &edx );
//    if ( ecx & ( 1 << 28 ) )
//    {
//        avx_support = true;
//    }
//
//    __get_cpuid( 7, &eax, &ebx, &ecx, &edx );
//    if ( ebx & ( 1 << 5 ) )
//    {
//        avx2_support = true;
//    }

    avx_support     = __builtin_cpu_supports( "avx" );
    avx2_support    = __builtin_cpu_supports( "avx2" );
}


cGrep::~cGrep()
{

}


void cGrep::startJobsProducer()
{
    if ( m_filePath != "" )
    {
        fileQ.push( m_filePath );

        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::string  dir( "." );
//    cFileFinder ff( fileQ, dir );
//    ff.start();

    thread t( cFileFinder::exploreDirectory, dir, &fileQ );

    t.detach();

    auto finish = std::chrono::high_resolution_clock::now();
//    std::cout << "Directory traverse took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;
//    cout << "Total records = " << fileQ.size() << endl;
}


void cGrep::startJobsConsumer()
{
    int workerThreads = 4;
    vector< thread >            pool;
    vector< vector< string > >  jobs;

    for ( auto i = 0; i < workerThreads; i ++ )
    {
        pool.push_back( thread( patternFinder, i, &fileQ, m_pattern ) );
    }

    for ( auto & thread : pool )
    {
        thread.join();
    }
}


void cGrep::start()
{
    startJobsProducer();

    startJobsConsumer();
}
