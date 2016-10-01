#include "cQueue.hpp"

using namespace std;

template <class T>
cQueue< T >::cQueue()
{

}


template <class T>
cQueue< T >::~cQueue()
{

}


template <class T>
void cQueue< T >::push( T data )
{
    lock_guard< mutex > lock( m_lock );

    m_queue.push( data );
}


template <class T>
T cQueue< T >::pop()
{
    lock_guard< mutex > lock( m_lock );

    auto data = m_queue.front();
    m_queue.pop();

    return data;
}


template class cQueue< string >;
