#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

struct Match
{
    Match( int line, int pos ) :
        line( line ), pos( pos )
    { }

    int line;
    int pos;
};


int main()
{
    int mapSize = 4096;
    int fd      = open( "big.txt", O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s = "woman";

    vector< Match > matches;

    for ( auto k = 0; k < size; k += mapSize )
    {
        auto data = mmap( 0, 4096, PROT_READ, MAP_SHARED, fd, k );

        char * t = ( char * ) data;

        int shash = 0;
        int thash = 0;
        int power = 1;

        int kMod = 997;
        int kBase = 26;

        int count = 1000;

        for ( auto i = 0; i < s.size(); i++ )
        {
            power = i ? power * kBase % kMod : 1;
            thash = ( thash * kBase + t[ i ] ) % kMod;
            shash = ( shash * kBase + s[ i ] ) % kMod;
        }

        int found = 0;
        for ( auto i = s.size(); i < mapSize; i++ )
        {
            if ( thash == shash )
            {
                char * val = &( t[ i - s.size() ] );

                if ( memcmp( s.c_str(), &t[ i - s.size() ], s.size() ) == 0 )
                {
                    matches.emplace_back( 0, k + i );
                }
            }

            thash -= ( t[ i - s.size() ] * power ) % kMod;
            if ( thash < 0 )
            {
                thash += kMod;
            }

            thash = ( thash * kBase + t[ i ] ) % kMod;
        }
    }

    cout << "Matches = " << matches.size() << endl;

    return 0;
}
