#pragma once

#include <vector>
#include <string>

#include "GrepCommon.hpp"

class iGrepEngine
{
public:
    virtual void reset( std::string & pattern ) = 0;
    virtual std::vector< sGrepMatchInstance > process( std::string & filename ) = 0;
};
