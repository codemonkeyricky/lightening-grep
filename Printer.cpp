#include <iostream>

#include "Printer.hpp"

using namespace std;

namespace Printer
{

void FileSummary(
    std::vector< SearcherI::sFileSummary >  & ssv
    )
{
    for ( auto & ss : ssv )
    {
        cout << "\033[1;32m" << ss.name << "\033[0m" << endl;

        for ( auto & r : ss.result )
        {
            cout << "\033[1;33m" << r.line <<
                "\033[0m" << " : " << r.content << endl;
        }

        cout << endl;
    }
}

};
