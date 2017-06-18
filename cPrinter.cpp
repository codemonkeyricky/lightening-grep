#include <unistd.h>

#include <cassert>
#include <iostream>

#include "cPrinter.hpp"

//Black        0;30     Dark Gray     1;30
//Red          0;31     Light Red     1;31
//Green        0;32     Light Green   1;32
//Brown/Orange 0;33     Yellow        1;33
//Blue         0;34     Light Blue    1;34
//Purple       0;35     Light Purple  1;35
//Cyan         0;36     Light Cyan    1;36
//Light Gray   0;37     White         1;37

#define COLOR_NONE          "\033[0m"
#define COLOR_LIGHT_CYAN    "\033[1;36m"
#define COLOR_YELLOW        "\033[1;33m"
#define COLOR_BG_BLUE       "\033[30;48;5;14m"

using namespace std;

std::mutex cPrinter::m_lock;

void cPrinter::print(
    sGrepFileSummary   &ss,
    std::string    &pattern
    )
{
    lock_guard< mutex > lock( m_lock );

//    ss.name.erase( 0, 2 );
    cout << COLOR_LIGHT_CYAN << ss.name << COLOR_NONE << endl;

    for ( auto & r : ss.result )
    {
        auto start = r.content.find( pattern ); 
        auto len = pattern.size(); 

        string toPrint; 
        if ( start != string::npos )
        {
            toPrint = 
                r.content.substr( 0, start ) + COLOR_BG_BLUE +
                r.content.substr( start, len ) + COLOR_NONE +
                r.content.substr( start + len );
        }
        else
        {
            // TODO: 
            // This is to work around if the result straddle between pages
            // and the pattern finder was too lazy to capture the entire line.
            
            toPrint = r.content; 
        }

        cout << COLOR_YELLOW << r.line <<
            COLOR_NONE << " : " << 
            toPrint << endl; 
    }

    cout << endl;
}


#if 0
void cPrinter::processor(
    sGrepSearchSummary &ss,
    std::string        &pattern
    )
{
    while ( true )
    {
        sGrepFileSummary fs;
        while ( ss.pop( fs ) )
        {
            print( fs, pattern );
        }

        usleep( 1 );
    }
}
#endif
