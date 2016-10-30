#pragma once

#include <mutex>
#include <queue>

#include "iQueue.hpp"

template < class T >
class cQueue : public iQueue< T >
{
public:
    cQueue();
    virtual ~cQueue();

    virtual bool push( T & );
    virtual bool pop( T & );
    virtual uint32_t size();

private:
    std::mutex      m_lock;
    std::queue< T > m_queue;
};

#include "tQueue.cpp"
