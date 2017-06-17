#pragma once

#include <string>
#include <vector>

#include "cQueue.hpp"
#include "sGrepCommon.hpp"

class cFileFinder
{
public:
    static void exploreDirectory(
        int                         workerThreads,
        std::string                 root,
        std::vector< std::string > &filter,
        iQueue< sGrepEntry >     &list
        );
};

