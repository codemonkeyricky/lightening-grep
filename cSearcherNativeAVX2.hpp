#pragma once

#include "iSearcher.hpp"

class cSearcherNativeAVX2 : public iSearcher
{
public:
    cSearcherNativeAVX2();
    ~cSearcherNativeAVX2();

    constexpr static int REGISTER_BIT_WIDTH    = 256;
    constexpr static int REGISTER_BYTE_WIDTH   = REGISTER_BIT_WIDTH / 8;

    virtual std::vector< sMatchInstance > process( std::string & filename, std::string & pattern );

private:
};
