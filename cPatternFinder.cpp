#include <vector>


#include "cPrinter.hpp"
#include "cPatternFinder.hpp"
#include "cGrepEngineNative.hpp"

using namespace std; 

void cPatternFinder::findPattern(
    int                     workerId,
    int                     cap,
    iQueue< sGrepEntry >   *fileList,
    string                  pattern
    )
{
    iGrepEngine  *searcher;
    cGrepEngineNative< AVX >  savx( pattern );
    cGrepEngineNative< AVX2 >  savx2( pattern );

    searcher = ( cap & static_cast< int >( Capability::AVX2 ) ) ?
        static_cast< iGrepEngine * >( &savx2 ) :
        static_cast< iGrepEngine * >( &savx );

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
            sFileSummary ssv( path.path, result );

            cPrinter::print( ssv, pattern );
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


