#include <vector>


#include "cPrinter.hpp"
#include "cSearcherNative.hpp"
#include "cPatternFinder.hpp"

using namespace std; 

extern int g_done; 

void cPatternFinder::findPattern(
    int                 workerId,
    int                 cap,
    iQueue< string >   *fileList,
    string              pattern
    )
{
    iSearcher  *searcher;
    cSearcherNative< AVX >  savx( pattern );
    cSearcherNative< AVX2 >  savx2( pattern );

    searcher = ( cap & static_cast< int >( Capability::AVX2 ) ) ?
        static_cast< iSearcher * >( &savx2 ) :
        static_cast< iSearcher * >( &savx );

    // auto start = std::chrono::high_resolution_clock::now();

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

//        usleep( 1 );
    }

    // auto finish = std::chrono::high_resolution_clock::now();

#if 0
    std::cout << "File Search took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " us" << endl;

    cout << "Worker ID = " << workerId << " " <<
        "Records processed = " << fileList->size() << " "
        "Match found = " << ssv.size() << endl;
#endif

    cPrinter::print( ssv );
}


