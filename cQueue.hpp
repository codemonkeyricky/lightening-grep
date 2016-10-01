#pragma once

#include <mutex>
#include <queue>

template <class T>
class cQueue
{
public:
    cQueue();
    ~cQueue();

    void push( T );
    T    pop();

private:
    std::mutex      m_lock;
    std::queue< T > m_queue;
};
