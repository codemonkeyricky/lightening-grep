#pragma once

#include <vector>
#include <string>

#include "sGrepCommon.hpp"

class iGrepEngine
{
public:

    int MMAP_SIZE   = 4096 * 4;

    virtual std::vector< sMatchInstance > process( std::string & filename ) = 0;
};
