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


struct sFunctionReference
{
    sFunctionReference( std::string & filename, std::string & funcname, uint32_t line ) 
        : filename( filename ), funcname( funcname ), line( line )
    { }

    std::string filename;
    std::string funcname;
    uint32_t    line; 
};


sFunctionReference findContainingFunction( 
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
    uint32_t    lineToReturn = lineNum;
    while ( curr < bm )
    {
        f.seekp( bm - curr ); 

        char c; 
        f.get( c );

        if ( mode == EXTRACT ) 
        {
            if ( c == ' ' || c == '\n' || c == '*' )
            {
                break; 
            }

            function += c; 
        }

        if ( c == '(' )
        {
            mode = EXTRACT; 
        }

        if ( c == '\n' )
        {
            lineToReturn --; 
        }

        curr ++; 
    }

    std::reverse( std::begin( function ), std::end( function ) ); 

    return sFunctionReference( filename, function, lineToReturn ); 
}


bool isLineCallReference(
    const std::string & line, 
    const std::string & pattern
    )
{
    // Line must start with an indent.
    if ( ! ( line.substr( 0, 4 ).compare( "    " ) == 0 || line.substr( 0, 1 ).compare( "\t" ) == 0 ) )
    {
        return false; 
    }

    // To qualify as a call ref pattern must be followed by '('.

    auto loc = line.find( pattern ); 
    if ( ( loc + pattern.length() ) < line.length() )
    {
        auto c = line[ loc + pattern.length() ]; 
#if 0
        if ( std::isalnum( c ) || c == '_' )
        {
            // Skip if next har is alpha numeric or '_'.

            return false; 
        }
#endif
        if ( c != '(' )
        {
            return false;
        } 
    }

    // Pattern must not be a substring of another function.

    if ( loc > 0 )
    {
        auto c = line[ loc - 1 ];
        if ( std::isalnum( c ) || c == '_' )
        {
            return false; 
        }
    } 

    // If line starts with '* ' then is a comment. 

    loc = 0;
    while ( loc < line.length() 
    && ( line[ loc ] == ' ' || line[ loc ] == '\t' ) )
    {
        loc ++; 
    }
    if ( line[ loc ] == '*' 
    && ( line[ loc + 1 ] == ' ' || line[ loc + 1 ] == '\t' ) )
    {
        return false; 
    }

    return true; 
}


std::vector< sFunctionReference > findCallers(
    const std::string & pattern
    )
{
    std::string path( "" );
    // std::string pattern( "cleancache_register_ops" );
    std::vector< std::string > filters = { "cc" };
    cGrepResultContainer    results; 
    cGrep grep( path, const_cast< std::string & >( pattern ), filters, &results );

    grep.start(); 

    std::vector< sFunctionReference > to_return; 

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

            if ( isLineCallReference( m.content, pattern ) )
            {
#if 0
                std::cout << s.name << std::endl; 
                std::cout << m.content << std::endl; 
#endif

                to_return.push_back( findContainingFunction( s.name, m.line ) ); 
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
    std::stack< std::vector< sFunctionReference > > callerStack; 
    std::string pattern; 
    pattern = argv[ 1 ]; 

    std::string name( "null" ); 
    std::vector< sFunctionReference >  initial = { { name, pattern, 0 } }; 
    callerStack.push( initial ); 

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

        // Pop the first entry in list.
        auto to_grep = callers[ 0 ]; 
        callers.erase( callers.begin() ); 

        // Print caller.
        auto indents = callerStack.size(); 
        for ( auto i = 0; i < indents; i++ )
        {
            std::cout << "    ";
        }
        std::cout << to_grep.funcname << " ( " << to_grep.filename << ":ln" << to_grep.line << " )" << std::endl;
        
        // Find all references & push if not empty.
        auto r = findCallers( to_grep.funcname ); 
        if ( r.size() > 0 )
        {
            callerStack.push( r ); 
        }
    }
}

