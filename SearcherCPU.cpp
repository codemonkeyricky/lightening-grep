#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <immintrin.h>
#include <assert.h>
#include <string.h>

#include <string>

using namespace std;

#include "SearcherCPU.hpp"

SearcherCPU::SearcherCPU()
{

}


SearcherCPU::~SearcherCPU()
{

}


uint32_t SearcherCPU::process(
    std::string & filename,
    std::string & pattern
    )
{
    int fd      = open( filename.c_str(), O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s    = pattern;

    int count   = 0;

    for ( auto k = 0; k < size; k += MMAP_SIZE )
    {
        auto data = mmap( 0, MMAP_SIZE, PROT_READ, MAP_SHARED, fd, k );

        char * t = ( char * ) data;

        int targetHash = 0;
        int rollingHash = 0;
        int power = 1;

        for ( auto i = 0; i < s.size(); i++ )
        {
            power       = i ? power * kBase % kMod : 1;
            rollingHash = ( rollingHash * kBase + t[ i ] ) % kMod;
            targetHash  = ( targetHash * kBase + s[ i ] ) % kMod;
        }

        int found = 0;
        for ( auto i = s.size(); i < MMAP_SIZE; i++ )
        {
            if ( rollingHash == targetHash )
            {
                char * val = &( t[ i - s.size() ] );

                if ( memcmp( s.c_str(), &t[ i - s.size() ], s.size() ) == 0 )
                {
                    count ++;
                }
            }

            rollingHash -= ( t[ i - s.size() ] * power ) % kMod;
            if ( rollingHash < 0 )
            {
                rollingHash += kMod;
            }

            rollingHash = ( rollingHash * kBase + t[ i ] ) % kMod;
        }

        munmap( data, MMAP_SIZE );
    }

    return count;
}
