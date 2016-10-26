#pragma once

#include <vector>
#include <string>

#include "cQueue.hpp"

class cGrep
{
public:
    cGrep( std::string &, std::string & );
    ~cGrep();

    void start();

    static bool avx_support;
    static bool avx2_support;

private:
    void startJobsProducer();
    void startJobsConsumer();

    static void patternFinder( int, cQueue< std::string > *, std::string );

    cQueue< std::string >       fileQ;

    std::string m_filePath;
    std::string m_pattern;
};
