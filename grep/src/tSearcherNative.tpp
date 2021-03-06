#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>

#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include "cGrepEngineNative.hpp"

using namespace std;

template< class T >
cGrepEngineNative<T>::cGrepEngineNative() 
{
}


template< class T >
cGrepEngineNative<T>::~cGrepEngineNative()
{

}


template< class T >
void cGrepEngineNative<T>::reset( 
    string & pattern 
    )
{
    m_pattern = pattern; 

    populatePatternVariables();
}



template< class T >
void cGrepEngineNative<T>::populatePatternVariables()
{
    alignas( ALIGNMENT ) char first8bitsRepeated[ simd_traits< T >::size ];
    memset( first8bitsRepeated, m_pattern[ 0 ], simd_traits< T >::size );
    firstLetterRepated = vector_load( ( const vec_type * ) &first8bitsRepeated[ 0 ] );

    alignas( ALIGNMENT ) char nl[ simd_traits< T >::size ];
    memset( nl, '\n', simd_traits< T >::size );
    nl_vec = vector_load( ( const vec_type * ) nl );

    alignas( ALIGNMENT ) char sp[ simd_traits< T >::size ][ PATTERN_SIZE_MAX ] = { 0 };

    for ( auto i = 0; i < simd_traits< T >::size; i++ )
    {
        memcpy( &( sp[ i ][ i ] ), m_pattern.c_str(), m_pattern.size() );
    }

    for ( auto i = 0; i < simd_traits< T >::size; i++ )
    {
        for ( auto j = 0; j < ( PATTERN_SIZE_MAX / simd_traits< T >::size ); j++ )
        {
            sp_vec[ i ][ j ] = vector_load( ( const vec_type * ) &sp[ i ][ j * simd_traits< T >::size ] );
        }
    }
}



template< class T >
void cGrepEngineNative<T>::insertRecord(
    const char                 *mmap,
    int                         mmapSize,
    const char                 *curr,
    const char                 *pattern,
    int                         currline,
    vector< sGrepMatchInstance >   &summary
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

    // Find the previous newline, but make sure we don't go out of bound.
    while ( start != mmap && *(--start) != '\n' ) { }
    if ( start != mmap || *start == '\n' ) 
    {
        // Account for the odd case where the newline is at the bound.
        start++;
    }

    // TODO: If the line start is at the previous page the accounting would be wrong. 

    // Find the end newline.
    while ( ( end != ( mmap + mmapSize ) ) && *(++end) != '\n' ) { }

    // TODO: If the line end is at the next page the accounting would be wrong. 

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
}


template< class T >
vector< sGrepMatchInstance > cGrepEngineNative<T>::process(
    string & filename
    )
{
    int fd          = open( filename.c_str(), O_RDONLY );
    int size        = lseek( fd, 0, SEEK_END );
    int patternSize = m_pattern.size();

    // Limit for now.
    assert( m_pattern.size() <= PATTERN_SIZE_MAX );

    vector< sGrepMatchInstance > summary;
    int ln = 0;

    alignas( ALIGNMENT ) char mm[ MMAP_SIZE + simd_traits< T >::size ];
    for ( auto offset = 0; offset < size; offset += MMAP_SIZE )
    {
        auto ms = std::min( ( long unsigned ) ( MMAP_SIZE + simd_traits< T >::size ), ( long unsigned ) ( size - offset ) );

        lseek( fd, offset, SEEK_SET );
        auto rd = read( fd, mm, ms );
        assert( ms == rd );

        for ( auto i = 0; i < ms; i += simd_traits< T >::size )
        {
            auto char32     = vector_load( ( const vec_type * ) ( mm + i ) );
            auto curr       = char32;

            auto match      = vector_compare( char32, firstLetterRepated );
            auto matchMask  = vector_to_bitmask( match );

            // Iterate through all mask bits.
            while ( matchMask )
            {
                // Find the bit offset.
                int patternOffset   = ffs( matchMask ) - 1;

                int bytesCompared   = 0;
                int iteration       = 0;
                while ( bytesCompared < patternSize )
                {
                    auto r              = vector_compare( char32, sp_vec[ patternOffset ][ iteration ] );

                    auto m2             = vector_to_bitmask( r );
                    int bytesMatched    = int_bits_count( m2 );

                    // Find matching letters required.
                    int bytesRemaining = patternSize - bytesCompared;

                    int regWidth = simd_traits< T >::size;
                    int bytesToMatch =
                        ( iteration == 0 ) ?
                            std::min( regWidth - patternOffset, patternSize ) :
                            std::min( regWidth, bytesRemaining );

                    // Failed to match.
                    if ( bytesMatched < bytesToMatch )
                    {
                        break;
                    }
                    else if ( bytesMatched > bytesToMatch )
                    {
                        auto of = i + iteration * simd_traits< T >::size + simd_traits< T >::size;
                        if ( ms < of )
                        {
                            bytesMatched -= of - ms;

                            // Loaded beyond end of file. False positives may hit due to null characters.
                            // Adjust matchedBytes accordingly.
                        }
                    }

                    // Accounting.
                    bytesCompared += bytesMatched;

                    // If all bytes match, then we found the string.
                    if ( bytesCompared == patternSize )
                    {
                        insertRecord( mm, MMAP_SIZE + simd_traits< T >::size, mm + i, m_pattern.c_str(), ln, summary );

                        break;
                    }

                    // Load the next 32 bytes.
                    char32  = vector_load( ( const vec_type * ) ( mm + i + ( ++ iteration ) * simd_traits< T >::size ) );
                }

                // Remove the lsb from mask.
                matchMask   &= matchMask - 1;
            }

            auto nls    = vector_compare( curr, nl_vec );
            auto nlm    = vector_to_bitmask( nls );
            auto lines  = int_bits_count( nlm );

            // TODO: This if() condition actually adds 5% to the execution time.
            // Not sure why. Is it the branch misprediction? Or simply because it
            // added a new instruction in the data path?
            //
            // This is required to ensure accuracy - will need to optimize later.
            if ( i < MMAP_SIZE )
            {
                ln += lines;
            }
        }
    }

    close( fd );

    return summary;
}


#if defined( __AVX__ ) 
template<>
inline __m128i cGrepEngineNative< AVX >::vector_load(
    const __m128i * input
    )
{
    return _mm_load_si128( input );
}


template<>
inline __m128i cGrepEngineNative< AVX >::vector_compare(
    __m128i & a,
    __m128i & b
    )
{
    return _mm_cmpeq_epi8( a, b );
}


template<>
inline unsigned int cGrepEngineNative< AVX >::vector_to_bitmask(
    __m128i & input
    )
{
    return _mm_movemask_epi8( input );
}


template<>
inline unsigned int cGrepEngineNative< AVX >::int_bits_count(
    unsigned int & input
    )
{
    return _mm_popcnt_u32( input );
}
#endif 


#if defined( __AVX2__ ) 
template<>
inline __m256i cGrepEngineNative< AVX2 >::vector_load(
    const __m256i * input
    )
{
    return _mm256_load_si256( input );
}

//
template<>
inline __m256i cGrepEngineNative< AVX2 >::vector_compare(
    __m256i & a,
    __m256i & b
    )
{
    return _mm256_cmpeq_epi8( a, b );
}


template<>
inline unsigned int cGrepEngineNative< AVX2 >::vector_to_bitmask(
    __m256i & input
    )
{
    return _mm256_movemask_epi8( input );
}


template<>
inline unsigned int cGrepEngineNative< AVX2 >::int_bits_count(
    unsigned int & input
    )
{
    return _mm_popcnt_u32( input );
}
#endif

