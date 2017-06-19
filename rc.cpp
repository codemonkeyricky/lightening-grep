// rc: reverse calltree utility 

#include <iostream>

#include "cGrep.hpp"

class cGrepResultContainer : public iGrepSearchSummary
{
public:
    cGrepResultContainer() { } 
    ~cGrepResultContainer() { } 

    virtual void push( sGrepFileSummary & fs )
    {
        std::cout << fs.name << std::endl; 
    }

    virtual bool pop( sGrepFileSummary & fs )
    {

    }
}; 

int main()
{
    std::string path( "../linux-4.8.15" );
    std::string pattern( "cleancache_register_ops" );
    std::vector< std::string > filters = { "cc" };
    cGrepResultContainer    results; 
    cGrep grep( path, pattern, filters, &results );

    grep.start(); 

    volatile int dummy = 0; 
}
