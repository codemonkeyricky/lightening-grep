#pragma once

#include <mutex>

#include "iSearcher.hpp"

class cPrinter
{
public:
    static void print( std::vector< iSearcher::sFileSummary >  & ssv );

    static void print( iSearcher::sFileSummary & ssv );

private:
    static std::mutex      m_lock;
};
