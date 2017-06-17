#pragma once

#include <string>

struct sGrepEntry
{
    enum class Msg : int
    {
        Search,
        Done
    };

    sGrepEntry() { }
    sGrepEntry( Msg msg, std::string & path ) : msg( msg ), path( path )
    { }

    Msg         msg;
    std::string path;
};



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
