#pragma once

#include "iQueue.hpp"
#include "sSearchCommon.hpp"

class cPatternFinder
{
public: 

    enum class Capability : int
    {
        AVX     = 0x01,
        AVX2    = 0x02
    };

    static void findPattern( int, int, iQueue< sSearchEntry > *, string );
}; 

