// rc: reverse calltree utility 

#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>

#include "cGrep.hpp"

class cGrepResultContainer : public iGrepSearchSummary
{
public:
    cGrepResultContainer() { } 
    ~cGrepResultContainer() { } 

    virtual void push( sGrepFileSummary & fs )
    {
        std::lock_guard< std::mutex > l( m_lock );

        m_queue.push( fs ); 

        // std::cout << fs.name << std::endl; 
    }

    virtual bool pop( sGrepFileSummary & fs )
    {
        std::lock_guard< std::mutex > l( m_lock );

        if ( m_queue.size() == 0 ) 
            return false; 

        fs = m_queue.front(); 
        m_queue.pop();

        return true;
    }

private: 
    std::mutex  m_lock;
    std::queue< sGrepFileSummary > m_queue; 
}; 


std::string findContainingFunction( 
    std::string    &filename, 
    uint32_t        lineNum
    )
{
    std::fstream f( filename ); 

    uint32_t curr = 0; 
    std::string line; 
    uint32_t bm;
    while ( curr < lineNum )
    {
        std::getline( f, line ); 

        if ( line[ 0 ] == '{' ) 
        {
            bm = f.tellp();
        }
    
        curr ++;
    }

    // bm points to beginning of function. 
    // Now extract function name.

    #define NONE    0
    #define EXTRACT 1

    curr = 0;
    int mode = NONE;
    std::string function; 
    while ( curr < bm )
    {
        f.seekp( bm - curr ); 

        char c; 
        f.get( c );

        if ( mode == EXTRACT ) 
        {
            if ( c == ' ' || c == '\n' )
            {
                break; 
            }

            function += c; 
        }

        if ( c == '(' )
        {
            mode = EXTRACT; 
        }

        curr ++; 
    }

    std::reverse( std::begin( function ), std::end( function ) ); 

    return function; 
}


bool isCallReference(
    const std::string & line, 
    const std::string & pattern
    )
{
    // Line must start with an indent.
    if ( ! ( line.substr( 0, 4 ).compare( "    " ) == 0 || line.substr( 0, 1 ).compare( "\t" ) == 0 ) )
    {
        return false; 
    }

    // Must contain character '('.
    if ( line.find( "(" ) == std::string::npos )
    {
        return false; 
    }

    // Pattern must not be a substring of another function.
    auto loc = line.find( pattern ); 
    if ( ( loc + pattern.length() ) < line.length() )
    {
        auto c = line[ loc + pattern.length() ]; 

        if ( std::isalnum( c ) || c == '_' )
        {
            // Skip if next har is alpha numeric or '_'.

            return false; 
        }
    }

    return true; 
}


std::vector< std::string > findCallers(
    const std::string & pattern
    )
{
    std::string path( "" );
    // std::string pattern( "cleancache_register_ops" );
    std::vector< std::string > filters = { "cc" };
    cGrepResultContainer    results; 
    cGrep grep( path, const_cast< std::string & >( pattern ), filters, &results );

    grep.start(); 

    std::vector< std::string > to_return; 

    sGrepFileSummary s; 
    while ( results.pop( s ) )
    {
        auto index = s.name.find_last_of( "." ); 
        auto extension = s.name.substr( index + 1 ); 

        // Ignore header files.
        if ( extension == "h" ) 
            continue; 

        // std::cout << s.name << std::endl; 

        for ( auto & m : s.result ) 
        {
            // Attempt to find function calls here. 

            if ( isCallReference( m.content, pattern ) )
            {
#if 0
                std::cout << s.name << std::endl; 
                std::cout << m.content << std::endl; 
#endif

                auto func = findContainingFunction( s.name, m.line );

                to_return.push_back( func ); 
            }
        }
    }

    return to_return;
}


int main(
    int argc, 
    char **argv
    )
{
    std::stack< std::vector< std::string > > callerStack; 
    std::string pattern; 
    pattern = argv[ 1 ]; 


    auto callers = findCallers( pattern );

    callerStack.push( callers ); 

    while ( callerStack.size() > 0 )
    {
        // Get top level list.
        auto & callers = callerStack.top(); 
        if ( callers.size() == 0 )
        {
            // If list empty pop and retry.
            callerStack.pop(); 

            continue;
        }

        // Get first entry in list & don't forget to remove.
        auto to_grep = callers[ 0 ]; 
        callers.erase( callers.begin() ); 
        
        // Find all references & push if not empty.
        auto r = findCallers( to_grep ); 
        if ( r.size() > 0 )
        {
            callerStack.push( r ); 
        }

        // Print caller.
        auto indents = callerStack.size(); 
        for ( auto i = 0; i < indents; i++ )
        {
            std::cout << "    ";
        }

        std::cout << to_grep << std::endl;
    }
}

