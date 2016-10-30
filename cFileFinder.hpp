#pragma once

#include <string>

#include "cQueue.hpp"
#include "sSearchCommon.hpp"

class cFileFinder
{
public:
    static void exploreDirectory( int workerThreads, std::string root, iQueue< sSearchEntry > *list );
};

