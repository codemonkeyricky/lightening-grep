#pragma once

#include <vector>
#include <immintrin.h>

#include "iSearcher.hpp"

class cSearcherNativeAVX2 : public iSearcher
{
public:
    cSearcherNativeAVX2( std::string & pattern );
    ~cSearcherNativeAVX2();

    constexpr static int REGISTER_BIT_WIDTH     = 256;
    constexpr static int REGISTER_BYTE_WIDTH    = REGISTER_BIT_WIDTH / 8;

    constexpr static int PATTERN_SIZE_MAX       = 64;
    constexpr static int REGISTERS_REQUIRED     = ( PATTERN_SIZE_MAX / REGISTER_BYTE_WIDTH );

    virtual std::vector< sMatchInstance > process( std::string & filename );

private:
    void populatePatternVariables();
    void insertRecord( const char *, const char *, int, std::vector< sMatchInstance > & );

    __m256i firstLetterRepated;
    __m256i nl256;
    __m256i sp_256[ REGISTER_BYTE_WIDTH ][ REGISTERS_REQUIRED ];

    std::string m_pattern;
};
