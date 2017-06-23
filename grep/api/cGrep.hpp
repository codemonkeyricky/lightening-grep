#pragma once

#include <vector>
#include <string>
#include <thread>

#include "GrepCommon.hpp"
#include "iQueue.hpp"

class cGrep
{
public:
    cGrep( 
        std::string                &path, 
        std::string                &pattern, 
        std::vector< std::string > &filters, 
        iGrepSearchSummary         *summary
        );
    ~cGrep();

    void start();

private:
    static bool avx_support;
    static bool avx2_support;

    void startProducer( std::vector< std::thread > &, iQueue< sGrepEntry > & );
    void startConsumer( std::vector< std::thread > &, iQueue< sGrepEntry > & );

    int                         m_workerThreads;
    std::string                 m_filePath;
    std::string                 m_pattern;
    std::vector< std::string >  m_filters;
    iGrepSearchSummary         *m_summary;
};
