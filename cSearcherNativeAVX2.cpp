#include "cSearcherNativeAVX2.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <immintrin.h>
#include "immintrin.h"
#include <assert.h>

#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

#define PATTERN_SIZE_MAX    64
#define REGISTERS_REQUIRED     ( PATTERN_SIZE_MAX / REGISTER_BYTE_WIDTH )

cSearcherNativeAVX2::cSearcherNativeAVX2()
{

}


cSearcherNativeAVX2::~cSearcherNativeAVX2()
{

}


vector< iSearcher::sMatchInstance > cSearcherNativeAVX2::process(
    string & filename,
    string & pattern
    )
{
    auto insertRecord = [] (
        const char   *curr,
        const char   *pattern,
        int     currline,
        vector< sMatchInstance > &summary
        )
    {
        // Pattern found.

        // Switch to scalar code for better maintenance.

        static int lastInsertedLn = -1;

        auto pos = strstr( curr, pattern );

        if ( pos == 0 )
            return;

        // Find the newlines preceding the pattern.
        int line = currline;
        while ( curr++ != pos )
            line += ( *curr == '\n' );

        auto start = pos;
        auto end = pos;

        // TODO: If the line straddles between page boundaries the code would
        // crash because the code below cannot find new lines.

        // Find the previous newline.
        while ( *(--start) != '\n' )
        { }
        start++;

        // Find the end newline.
        while ( *(++end) != '\n' )
        { }

        auto content = string( start, end - start );

        // Multiple matches on one line only needs to be inserted once.
        if ( line + 1 != lastInsertedLn )
        {
            summary.emplace_back(
                line + 1,
                pos - start,
                content
            );
        }

        lastInsertedLn = line + 1;
    };

    int fd      = open( filename.c_str(), O_RDONLY );
    int size    = lseek( fd, 0, SEEK_END );
    string s    = pattern;
    int patternSize = pattern.size();

    // Limit for now.
    assert( pattern.size() <= REGISTER_BYTE_WIDTH );

    vector< sMatchInstance > summary;
    int ln = 0;

    int count = 0;

    alignas( REGISTER_BYTE_WIDTH ) char first8bitsRepeated[ REGISTER_BYTE_WIDTH ];
    memset( first8bitsRepeated, pattern[ 0 ], REGISTER_BYTE_WIDTH );
    auto firstLetterRepated = _mm256_loadu_si256( ( const __m256i * ) &first8bitsRepeated[ 0 ] );

    alignas( REGISTER_BYTE_WIDTH ) char nl[ REGISTER_BYTE_WIDTH ];
    memset( nl, '\n', REGISTER_BYTE_WIDTH );
    auto nl256 = _mm256_loadu_si256( ( const __m256i * ) nl );

    alignas( REGISTER_BYTE_WIDTH ) char sp[ REGISTER_BYTE_WIDTH ][ PATTERN_SIZE_MAX ] = { 0 };
    __m256i sp_256[ REGISTER_BYTE_WIDTH ][ REGISTERS_REQUIRED ];

    for ( auto i = 0; i < REGISTER_BYTE_WIDTH; i++ )
    {
        memcpy( &( sp[ i ][ i ] ), pattern.c_str(), pattern.size() );
    }

    for ( auto i = 0; i < REGISTER_BYTE_WIDTH; i++ )
    {
        for ( auto j = 0; j < REGISTERS_REQUIRED; j++ )
        {
            sp_256[ i ][ j ] = _mm256_loadu_si256( ( const __m256i * ) &sp[ i ][ j * REGISTER_BYTE_WIDTH ] );
        }
    }

    alignas( REGISTER_BYTE_WIDTH ) char mm[ MMAP_SIZE ];
    for ( auto offset = 0; offset < size; offset += MMAP_SIZE )
    {
        auto ms = std::min( MMAP_SIZE + REGISTER_BYTE_WIDTH, size - offset );

        lseek( fd, offset, SEEK_SET );
        auto rd = read( fd, mm, ms );
        assert( ms == rd );

        for ( auto i = 0; i < ms; i += REGISTER_BYTE_WIDTH )
        {
            auto char32     = _mm256_load_si256( ( const __m256i * ) ( mm + i ) );
            auto curr       = char32;

            auto match      = _mm256_cmpeq_epi8( char32, firstLetterRepated );
            auto matchMask  = _mm256_movemask_epi8( match );

            // Iterate through all mask bits.
            while ( matchMask )
            {
                // Find the bit offset.
                int patternOffset   = ffs( matchMask ) - 1;

                int bytesCompared   = 0;
                int iteration       = 0;
                while ( bytesCompared < patternSize )
                {
                    auto r              = _mm256_cmpeq_epi8( char32, sp_256[ patternOffset ][ iteration ] );

                    auto m2             = _mm256_movemask_epi8( r );
                    int matchedBytes    = _mm_popcnt_u32( m2 );

                    // Find matching letters required.
                    int bytesRemaining = patternSize - bytesCompared;

                    int bytesRequiredToMatch = ( iteration == 0 ) ?
                        std::min( REGISTER_BYTE_WIDTH - patternOffset, patternSize ) : bytesRemaining;

                    // Failed to match.
                    if ( matchedBytes < bytesRequiredToMatch )
                    {
                        break;
                    }
                    else if ( matchedBytes > bytesRequiredToMatch )
                    {
                        auto of = i + iteration * REGISTER_BYTE_WIDTH + REGISTER_BYTE_WIDTH;
                        if ( ms < of )
                        {
                            matchedBytes -= of - ms;

                            // Loaded beyond end of file. False positives may hit due to null characters.
                            // Adjust matchedBytes accordingly.
                        }
                    }

                    // Accounting.
                    bytesCompared += matchedBytes;

                    // If all bytes match, then we found the string.
                    if ( bytesCompared == patternSize )
                    {
                        insertRecord( mm + i, pattern.c_str(), ln, summary );

                        break;
                    }

                    // Load the next 32 bytes.
                    char32  = _mm256_load_si256( ( const __m256i * ) ( mm + i + ( ++ iteration ) * REGISTER_BYTE_WIDTH ) );

//                    patternOffset = bytesRequiredToMatch;
                }

                // Remove the lsb from mask.
                matchMask   &= matchMask - 1;
            }

            auto nls    = _mm256_cmpeq_epi8( curr, nl256 );
            auto nlm    = _mm256_movemask_epi8( nls );
            auto lines  = _mm_popcnt_u32( nlm );

            ln  += lines;
        }

        munmap( mm, ms );
    }

    close( fd );

    return summary;
}
