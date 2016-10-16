#pragma once

#include "iSearcher.hpp"

class SearcherCPU : public iSearcher
{
public:
    SearcherCPU();
    ~SearcherCPU();

    uint32_t    process( std::string & filename, std::string & pattern );
};
