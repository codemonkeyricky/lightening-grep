#pragma once

#include "SearcherI.hpp"

class SearcherSSE : public SearcherI
{
public:
    SearcherSSE();
    ~SearcherSSE();

    uint32_t process( std::string & filename, std::string & pattern );

private:
};
