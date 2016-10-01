#pragma once

#include "SearcherI.hpp"

class SearcherAVX2 : public SearcherI
{
public:
    SearcherAVX2();
    ~SearcherAVX2();

    uint32_t process( std::string & filename, std::string & pattern );

private:
    int kMod    = 0x800000;
    int kBase   = 129;
};
