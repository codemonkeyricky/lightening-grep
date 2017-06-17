#pragma once

#include <mutex>

#include "iGrepEngine.hpp"

class cPrinter
{
public:
    static void print( std::vector< iGrepEngine::sFileSummary > &, std::string & );

    static void print( iGrepEngine::sFileSummary &, std::string & );

private:
    static std::mutex      m_lock;
};
