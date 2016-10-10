#pragma once

#include <vector>
#include <string>

class SearcherI
{
public:

    struct sMatchInstance
    {
        sMatchInstance( uint32_t line, uint32_t offset, std::string & content ) :
            line( line ), offset( offset ), content( content )
        { }

        uint32_t    line;
        uint32_t    offset;
        std::string content;
    };

    struct sFileSummary
    {
        sFileSummary(
            std::string             & name,
            std::vector< sMatchInstance > & result
        ) : name( name ), result( result )
        { }

        std::string             name;
        std::vector< sMatchInstance > result;
    };

    int MMAP_SIZE   = 4096 * 4;

    virtual std::vector< sMatchInstance > process( std::string & filename, std::string & pattern ) = 0;
};
