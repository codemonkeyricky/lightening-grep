#pragma once

#include <atomic>

#include "iQueue.hpp"

template < class T >
class cQueueLockless : public iQueue< T >
{
#define MAX_SIZE 128

public:
    cQueueLockless();
    virtual ~cQueueLockless();

    virtual bool push( T & );
    virtual bool pop( T & );
    virtual uint32_t size();

private:
    int                 m_totalSize;
    std::atomic< int >  m_read;
    std::atomic< int >  m_write;
    T                   m_data[ MAX_SIZE ];
};
