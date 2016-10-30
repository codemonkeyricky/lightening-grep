#pragma once

#include <string>

struct sSearchEntry
{
    enum class Msg : int
    {
        Search,
        Done
    };

    sSearchEntry() { }
    sSearchEntry( Msg msg, std::string & path ) : msg( msg ), path( path )
    { }

    Msg     msg;
    std::string  path;
};
