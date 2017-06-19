#pragma once

#include <mutex>
#include <iostream>

#include "GrepCommon.hpp"

#define COLOR_NONE          "\033[0m"
#define COLOR_LIGHT_CYAN    "\033[1;36m"
#define COLOR_YELLOW        "\033[1;33m"
#define COLOR_BG_BLUE       "\033[30;48;5;14m"

class cPrinter
{
public:
    static void print( sGrepFileSummary &ss, std::string &pattern )
    {
        std::lock_guard< std::mutex > lock( m_lock );

        std::cout << COLOR_LIGHT_CYAN << ss.name << COLOR_NONE << std::endl;

        for ( auto & r : ss.result )
        {
            auto start = r.content.find( pattern ); 
            auto len = pattern.size(); 

            std::string toPrint; 
            if ( start != std::string::npos )
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

            std::cout << COLOR_YELLOW << r.line <<
                COLOR_NONE << " : " << 
                toPrint << std::endl; 
        }

        std::cout << std::endl;
    }
    
private:
    static std::mutex   m_lock;
};
std::mutex cPrinter::m_lock;
