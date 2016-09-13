#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string>

using namespace std;

int main()
{
    int fd = open( "big.txt", O_RDONLY );

    volatile auto data = mmap( 0, 4096, PROT_READ, MAP_SHARED, fd, 0 );

    char * t = ( char * ) data;

    string s = "Copyright";

    int shash = 0;
    int thash = 0;
    int power = 1;

    int kMod = 997;
    int kBase = 26;

    for ( auto i = 0; i < s.size(); i++ )
    {
        power = i ? power * kBase % kMod : 1;
        thash = ( thash * kBase + t[ i ] ) % kMod;
        shash = ( shash * kBase + s[ i ] ) % kMod;
    }

    int found = 0;
    for ( auto i = s.size(); i < 4096; i++ )
    {
        if ( thash == shash )
        {
            char * val = &( t[ i - s.size() ] );

            found = 1;
//            break;
        }

        thash -= ( t[ i - s.size() ] * power ) % kMod;
        if ( thash < 0 )
        {
            thash += kMod;
        }

        thash = ( thash * kBase + t[ i ] ) % kMod;
    }

    return 0;
}
