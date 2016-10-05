#pragma once

#include <vector>
#include <string>

class SearcherI
{
public:

    struct Instance
    {
        uint32_t    line;
        uint32_t    offset;
        std::string content;

        Instance( uint32_t line, uint32_t offset, std::string & content ) :
            line( line ), offset( offset ), content( content )
        { }
    };

    int kMod        = 0x800000;
    int kBase       = 129;
    int MMAP_SIZE   = 4096 * 4;

    virtual std::vector< Instance > process( std::string & filename, std::string & pattern ) = 0;
};
