#pragma once

#include <mutex>

#include "iSearcher.hpp"

class cPrinter
{
public:
    static void print( std::vector< iSearcher::sFileSummary > &, std::string & );

    static void print( iSearcher::sFileSummary &, std::string & );

private:
    static std::mutex      m_lock;
};
