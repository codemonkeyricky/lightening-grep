#pragma once

#include <vector>
#include <string>

#include "iQueue.hpp"
#include "sSearchCommon.hpp"

class cGrep
{
public:
    cGrep( std::string &, std::string & );
    ~cGrep();

    void start();

    static bool avx_support;
    static bool avx2_support;

private:
    void startProducer( std::vector< std::thread > &, iQueue< sSearchEntry > & );
    void startConsumer( std::vector< std::thread > &, iQueue< sSearchEntry > & );

    int         m_workerThreads = 4;
    std::string m_filePath;
    std::string m_pattern;
};
