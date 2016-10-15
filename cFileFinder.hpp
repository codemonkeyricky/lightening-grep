#pragma once

#include <string>

#include "cQueue.hpp"

class cFileFinder
{
public:
    cFileFinder( std::vector< std::string > &q, std::string & path );
    ~cFileFinder();

    void start();
    void stop();

    struct Path
    {
        std::string prefix;
//        std::string
    };

private:
    std::vector< std::string >      &m_fileList;
    std::queue< std::string >   m_pathToExplore;
    std::string                 m_rootPath;
};
