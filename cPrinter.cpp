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
#define COLOR_BG_BLUE       "\033[48;5;27m"

using namespace std;

std::mutex cPrinter::m_lock;


void cPrinter::print(
    iSearcher::sFileSummary & ss,
    std::string             & pattern
    )
{
    lock_guard< mutex > lock( m_lock );

    cout << COLOR_LIGHT_CYAN << ss.name << COLOR_NONE << endl;

    for ( auto & r : ss.result )
    {
        auto start = r.content.find( pattern ); 
        auto len = pattern.size(); 

        cout << COLOR_YELLOW << r.line <<
            COLOR_NONE << " : " << 
            r.content.substr( 0, start ) << 
            COLOR_BG_BLUE << 
            r.content.substr( start, len ) << 
            COLOR_NONE << 
            r.content.substr( start + len ) << 
            endl;
    }

    cout << endl;
}


void cPrinter::print(
    std::vector< iSearcher::sFileSummary >  & ssv, 
    std::string                             & pattern
    )
{
#if 0
    lock_guard< mutex > lock( m_lock );

    for ( auto & ss : ssv )
    {
        print( ss, pattern );

        cout << endl;
    }
#endif

    assert( 0 );
}
