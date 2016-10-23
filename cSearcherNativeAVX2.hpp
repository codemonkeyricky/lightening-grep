#pragma once

#include <vector>
#include <immintrin.h>

#include "iSearcher.hpp"

constexpr int PATTERN_SIZE_MAX       = 64;
constexpr int ALIGNMENT              = 32;

template< typename T >
class cSearcherNativeAVX2 : public iSearcher
{
public:

#define REGISTER_BYTE_WIDTH ( sizeof( T ) )
#define REGISTERS_REQUIRED  ( PATTERN_SIZE_MAX / REGISTER_BYTE_WIDTH )

    cSearcherNativeAVX2( std::string & pattern );
    ~cSearcherNativeAVX2();

    inline T vector_load( const T * );
    inline T vector_compare( T &, T & );
    inline unsigned int vector_to_bitmask( T & );
    inline unsigned int int_bits_count( unsigned int & );

    virtual std::vector< sMatchInstance > process( std::string & filename );

private:

    void populatePatternVariables();
    void insertRecord( const char *, const char *, int, std::vector< sMatchInstance > & );

    T firstLetterRepated;
    T nl_vec;
    T sp_vec[ REGISTER_BYTE_WIDTH ][ REGISTERS_REQUIRED ];

    std::string m_pattern;
};

//template <> inline __m128i cSearcherNativeAVX2< __m128i >::vector_load( const __m128i * );
//template <> inline __m128i cSearcherNativeAVX2< __m128i >::vector_compare( __m128i &, __m128i & );
//template <> inline unsigned int cSearcherNativeAVX2< __m128i >::vector_to_bitmask( __m128i & );
//template <> inline unsigned int cSearcherNativeAVX2< __m128i >::int_bits_count( unsigned int & );
//
//template <> inline __m256i cSearcherNativeAVX2< __m256i >::vector_load( const __m256i * );
//template <> inline __m256i cSearcherNativeAVX2< __m256i >::vector_compare( __m256i &, __m256i & );
//template <> inline unsigned int cSearcherNativeAVX2< __m256i >::vector_to_bitmask( __m256i & );
//template <> inline unsigned int cSearcherNativeAVX2< __m256i >::int_bits_count( unsigned int & );
