#pragma once

#include <vector>
#include <string>
#include <thread>

#include "GrepCommon.hpp"
#include "iQueue.hpp"

class cGrep
{
public:
    cGrep( std::string &, std::string &, std::vector< std::string > &, iGrepSearchSummary * );
    ~cGrep();

    void start();

    static bool avx_support;
    static bool avx2_support;

private:
    void startProducer( std::vector< std::thread > &, iQueue< sGrepEntry > & );
    void startConsumer( std::vector< std::thread > &, iQueue< sGrepEntry > & );

    int                         m_workerThreads = 4;
    std::string                 m_filePath;
    std::string                 m_pattern;
    std::vector< std::string >  m_filters;
    iGrepSearchSummary         *m_summary;
};
