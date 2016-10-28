#pragma once

#include <string>

#include "cQueue.hpp"

class cFileFinder
{
public:
    static void exploreDirectory( std::string root, iQueue< std::string > *list );
};

