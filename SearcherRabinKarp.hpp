#pragma once

#include "SearcherI.hpp"

class SearcherCPU : public SearcherI
{
public:
    SearcherCPU();
    ~SearcherCPU();

    uint32_t    process( std::string & filename, std::string & pattern );
};
