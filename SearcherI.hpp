#pragma once

#include <vector>
#include <string>

class SearcherI
{
public:

    struct Result
    {
        uint32_t    line;
        uint32_t    offset;
        std::string      content;
    };

    int kMod        = 0x800000;
    int kBase       = 129;
    int MMAP_SIZE   = 4096;

    virtual uint32_t process( std::string & filename, std::string & pattern ) = 0;
};
