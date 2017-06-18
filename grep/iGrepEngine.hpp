#pragma once

#include <vector>
#include <string>

#include "GrepCommon.hpp"

class iGrepEngine
{
public:

    int MMAP_SIZE   = 4096 * 4;

    virtual std::vector< sGrepMatchInstance > process( std::string & filename ) = 0;
};
