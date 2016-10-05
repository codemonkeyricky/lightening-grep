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
bool cQueue< T >::pop( T & data )
{
    lock_guard< mutex > lock( m_lock );

    if ( m_queue.empty() )
    {
        return false;
    }

    data = m_queue.front();
    m_queue.pop();

    return true;
}


template <class T>
uint32_t cQueue< T >::size()
{
    lock_guard< mutex > lock( m_lock );

    return m_queue.size();
}


template class cQueue< string >;
