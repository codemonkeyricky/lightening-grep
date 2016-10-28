#pragma once

#include <string>

#include "cQueue.hpp"

class cFileFinder
{
public:
    cFileFinder( cQueue< std::string > &q, std::string & path );
    ~cFileFinder();

    void start();
    void stop();

    struct Path
    {
        std::string prefix;
//        std::string
    };

    static void exploreDirectory( std::string root, iQueue< std::string > *list );

private:
    cQueue< std::string >      &m_fileList;
    std::queue< std::string >   m_pathToExplore;
    std::string                 m_rootPath;
};
