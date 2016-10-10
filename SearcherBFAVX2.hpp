#pragma once

#include "SearcherI.hpp"

class SearcherBFAVX2 : public SearcherI
{
public:
    SearcherBFAVX2();
    ~SearcherBFAVX2();

    virtual std::vector< sMatchInstance > process( std::string & filename, std::string & pattern );

private:
};
