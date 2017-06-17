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
