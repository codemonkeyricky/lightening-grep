
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

void cGrep::patternFinder(
    int                 workerId,
    vector< string >   *fileList,
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

    string path;
    for ( auto & path : *fileList )
    {
        auto result = searcher->process( path );

        if ( result.size() > 0 )
            ssv.emplace_back( path, result );
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


void cGrep::populateJobQueue()
{
    if ( m_filePath != "" )
    {
        fileQ.push_back( m_filePath );

        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::string  dir( "." );
    cFileFinder ff( fileQ, dir );
    ff.start();

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Directory traverse took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;
    cout << "Total records = " << fileQ.size() << endl;
}


void cGrep::startJobs()
{
    int workerThreads = 4;
    vector< thread >            pool;
    vector< vector< string > >  jobs;
    jobs.reserve( workerThreads );
    int jobsPerWorker = fileQ.size() / workerThreads;
    for ( auto i = 0; i < workerThreads; i ++ )
    {
        auto start = fileQ.begin() + jobsPerWorker * i;
        auto end = ( i != ( workerThreads - 1 ) ) ?
            ( fileQ.begin() + jobsPerWorker * ( i + 1 ) ) :
            fileQ.end();

        jobs.emplace_back( start, end );

        pool.push_back( thread( patternFinder, i, &( jobs.back() ), m_pattern ) );
    }

    for ( auto & thread : pool )
    {
        thread.join();
    }
}


void cGrep::start()
{
    populateJobQueue();

    startJobs();
}
