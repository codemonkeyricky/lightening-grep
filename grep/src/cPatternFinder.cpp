#include <vector>


#include "cPatternFinder.hpp"
#include "cGrepEngineNative.hpp"

using namespace std; 

struct sProcessEngine
{
    cGrepEngineNative< AVX >  savx; 
    cGrepEngineNative< AVX2 >  savx2;
}; 

static std::vector< sProcessEngine >    f_processEngines;

void cPatternFinder::reset(
    int workers
    )
{
    f_processEngines.resize( workers ); 
}

void cPatternFinder::findPattern(
    int                     workerId,
    int                     cap,
    iQueue< sGrepEntry >   *fileList,
    string                  pattern,
    iGrepSearchSummary     *summary
    )
{
    iGrepEngine  *searcher;

    searcher = ( cap & static_cast< int >( Capability::AVX2 ) ) ?
        static_cast< iGrepEngine * >( &f_processEngines[ workerId ].savx2 ) :
        static_cast< iGrepEngine * >( &f_processEngines[ workerId ].savx );

    searcher->reset( pattern );

    // auto start = std::chrono::high_resolution_clock::now();

    while ( true )
    {
        sGrepEntry path;
        if ( !fileList->pop( path ) )
        {
            usleep( 1 );

            continue;
        }

        if ( path.msg == sGrepEntry::Msg::Done )
            break;

        auto result = searcher->process( path.path );

        if ( result.size() > 0 )
        {
            sGrepFileSummary ssv( path.path, result );

            summary->push( ssv );

        //    cPrinter::print( ssv, pattern );
        }
    }

    // auto finish = std::chrono::high_resolution_clock::now();

#if 0
    std::cout << "File Search took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;

    cout << "Worker ID = " << workerId << " " <<
        "Records processed = " << fileList->size() << " "
        "Match found = " << ssv.size() << endl;
#endif

//    cPrinter::print( ssv );
}


