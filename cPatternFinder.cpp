#include <vector>


#include "cPrinter.hpp"
#include "cSearcherNative.hpp"
#include "cPatternFinder.hpp"

using namespace std; 

void cPatternFinder::findPattern(
    int                     workerId,
    int                     cap,
    iQueue< sSearchEntry > *fileList,
    string                  pattern
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

    while ( true )
    {
        sSearchEntry path;
        if ( !fileList->pop( path ) )
        {
            usleep( 1 );

            continue;
        }

        if ( path.msg == sSearchEntry::Msg::Done )
            break;

        auto result = searcher->process( path.path );

        if ( result.size() > 0 )
        {
            iSearcher::sFileSummary ssv( path.path, result );

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


