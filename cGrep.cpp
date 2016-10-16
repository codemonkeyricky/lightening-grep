#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "cGrep.hpp"
#include "cFileFinder.hpp"
#include "Printer.hpp"
#include "SearcherNativeAVX2.hpp"

using namespace std;

void patternFinder(
    vector< string >   * fileList,
    string              pattern
    )
{
    SearcherNativeAVX2  searcher;

    auto start = std::chrono::high_resolution_clock::now();

    vector< SearcherI::sFileSummary > ssv;

    string path;
    for ( auto & path : *fileList )
    {
        auto result = searcher.process( path, pattern );

        if ( result.size() > 0 )
            ssv.emplace_back( path, result );
    }

    auto finish = std::chrono::high_resolution_clock::now();

#if 0
    std::cout << "File Search took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;
    cout << "Total records processed = " << count << endl;
#endif

    Printer::FileSummary( ssv );
}


cGrep::cGrep(
    std::string & filePath,
    std::string & pattern
    )
: m_filePath( filePath ),
  m_pattern( pattern )
{

}


cGrep::~cGrep()
{

}


void cGrep::populateFileList()
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


void cGrep::start()
{
    populateFileList();


    int workerThreads = 4;
    vector< thread >            pool;
    vector< vector< string > >  jobs;
    int jobLen = fileQ.size() / workerThreads;
    for ( auto i = 0; i < workerThreads; i ++ )
    {
        auto start = fileQ.begin() + jobLen * i;
        auto end = ( i != workerThreads - 1 ) ?
            ( fileQ.begin() + jobLen * ( i + 1 ) ) :
            fileQ.end();

        jobs.emplace_back( start, end );

        pool.push_back( thread( patternFinder, &jobs.back(), m_pattern ) );
    }

    for ( auto & thread : pool )
    {
        thread.join();
    }
}
