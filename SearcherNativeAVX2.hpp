#pragma once

#include "SearcherI.hpp"

class SearcherNativeAVX2 : public SearcherI
{
public:
    SearcherNativeAVX2();
    ~SearcherNativeAVX2();

    constexpr static int REGISTER_BIT_WIDTH    = 256;
    constexpr static int REGISTER_BYTE_WIDTH   = REGISTER_BIT_WIDTH / 8;

    virtual std::vector< sMatchInstance > process( std::string & filename, std::string & pattern );

private:
};
