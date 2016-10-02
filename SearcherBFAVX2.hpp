#pragma once

#include "SearcherI.hpp"

class SearcherBFAVX2 : public SearcherI
{
public:
    SearcherBFAVX2();
    ~SearcherBFAVX2();

    virtual std::vector< Instance > process( std::string & filename, std::string & pattern );

private:
};
