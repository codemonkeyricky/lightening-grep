#pragma once

#include <vector>
#include <string>

class cGrep
{
public:
    cGrep( std::string &, std::string & );
    ~cGrep();

    void start();

    static bool avx_support;
    static bool avx2_support;

private:
    void populateJobQueue();
    void startJobs();

    static void patternFinder( int, std::vector< std::string > *, std::string );

    std::vector< std::string >    fileQ;

    std::string m_filePath;
    std::string m_pattern;
};
