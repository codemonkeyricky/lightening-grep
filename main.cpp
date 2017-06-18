#include <cstring>

#include "cGrep.hpp"
#include "cPrinter.hpp"

using namespace std;

class cGrepSearchSummaryPrinter : public iGrepSearchSummary
{
public:
    cGrepSearchSummaryPrinter( std::string & pattern )
        : m_pattern( pattern )
    { }

    ~cGrepSearchSummaryPrinter() { }

    virtual void push( sGrepFileSummary & fs )
    {
        cPrinter::print( fs, m_pattern );
    }

    virtual bool pop( sGrepFileSummary & fs )
    { }

private:
    std::string m_pattern;
};

int main(
    int argc,
    char **argv
    )
{
    if ( argc < 2 )
    {
        return -1;
    }

    std::vector< std::string > filters; 
    string filter;

    int index = 1;
    string pattern;
    string path;
    while ( index < argc )
    {
        // Parse options.
        if ( strncmp( argv[ index ], "-", 1 ) == 0 )
        {
            // Filter options.
            if ( strncmp( argv[ index ], "--", 2 ) == 0 )
            {
                filter = string( argv[ index ] + 2 );

                filters.emplace_back( filter );
            }
        }
        else
        if ( pattern == "" )
        {
            pattern = argv[ index ];
        }
        else
        {
            if ( path == "" )
            {
                path = argv[ index ];
            }
            else
            {
                return -1;
            }
        }

        index ++;
    }

    cGrepSearchSummaryPrinter  ss( pattern );
    cGrep grepper( path, pattern, filters, &ss );
    grepper.start();

    return 0;
}
