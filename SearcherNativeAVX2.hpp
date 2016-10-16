#pragma once

#include "SearcherI.hpp"

class SearcherNativeAVX2 : public SearcherI
{
public:
    SearcherNativeAVX2();
    ~SearcherNativeAVX2();

    virtual std::vector< sMatchInstance > process( std::string & filename, std::string & pattern );

private:
};
