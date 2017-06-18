#pragma once

#include <mutex>

#include "GrepCommon.hpp"
#include "iGrepEngine.hpp"

class cPrinter
{
public:
    static void print( sGrepFileSummary &, std::string & );
    static void processor( sGrepSearchSummary &, std::string & );

private:
    static std::mutex   m_lock;
};
