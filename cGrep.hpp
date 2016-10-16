#pragma once

#include <vector>
#include <string>

class cGrep
{
public:
    cGrep( std::string &, std::string & );
    ~cGrep();

    void start();

private:
    void populateJobQueue();
    void startJobs();

    std::vector< std::string >    fileQ;

    std::string m_filePath;
    std::string m_pattern;
};
