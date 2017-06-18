#pragma once

#include <string>
#include <vector>
#include <queue>
#include <mutex>

struct sGrepEntry
{
    enum class Msg : int
    {
        Search,
        Done
    };

    sGrepEntry() { }
    sGrepEntry( Msg msg, std::string & path ) : msg( msg ), path( path )
    { }

    Msg         msg;
    std::string path;
};

struct sGrepMatchInstance
{
    sGrepMatchInstance( uint32_t line, uint32_t offset, std::string & content ) :
        line( line ), offset( offset ), content( content )
    { }

    uint32_t    line;
    uint32_t    offset;
    std::string content;
};

struct sGrepFileSummary
{
    sGrepFileSummary() { }

    sGrepFileSummary(
            std::string                        & name,
            std::vector< sGrepMatchInstance >  & result
    ) : name( name ), result( result )
    { }

    std::string                         name;
    std::vector< sGrepMatchInstance >   result;
};


class iGrepSearchSummary
{
public: 
    iGrepSearchSummary() { } 
    ~iGrepSearchSummary() { } 

    virtual void push( sGrepFileSummary & fs ) = 0; 
    virtual bool pop( sGrepFileSummary & fs ) = 0; 
}; 


#if 0
class sGrepSearchSummary : public iGrepSearchSummary
{
public:
    sGrepSearchSummary() { }
    ~sGrepSearchSummary() { }

    void push( sGrepFileSummary & fs )
    {
        std::lock_guard< std::mutex > l( m_lock );

        result.emplace( fs );
    }

    bool pop( sGrepFileSummary & fs )
    {
        while ( true )
        {
            std::unique_lock< std::mutex > l( m_lock, std::try_to_lock );

            if ( l.owns_lock() )
            {
                if ( result.size() == 0 )
                {
                    return false;
                }

                fs = result.front();
                result.pop();

                return true;
            }
        }
    }

private:
    std::mutex                      m_lock;
    std::queue< sGrepFileSummary >  result;
};
#endif
