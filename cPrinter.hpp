#pragma once

#include <mutex>

#include "iGrepEngine.hpp"
#include "sGrepCommon.hpp"

class cPrinter
{
public:
    static void print( std::vector< sFileSummary > &, std::string & );

    static void print( sFileSummary &, std::string & );

private:
    static std::mutex      m_lock;
};
