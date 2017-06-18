#include "cQueueLockless.hpp"
#include "string.h"


template <class T>
cQueueLockless< T >::cQueueLockless() :
    m_totalSize( 128 ), 
    m_read( 0 ), 
    m_write( 0 )
{
    memset( ( void * ) m_data, 0, sizeof( T ) * 128 );
}


template <class T>
cQueueLockless< T >::~cQueueLockless()
{
}


template <class T>
bool cQueueLockless< T >::push(
    T   &data
    )
{
    auto read_index         = m_read.load( std::memory_order_relaxed ); 
    auto write_index        = m_write.load( std::memory_order_relaxed ); 
    auto next_write_index   = ( write_index + 1 ) % m_totalSize;

    if ( next_write_index == read_index )
    {
        // Ring is full.

        return false;
    }

    // Put the data.
    m_data[ write_index ] = data;

    // Update the write index.
    m_write.store( next_write_index, std::memory_order_release );

    // Success.

    return true;
}


template <class T>
bool cQueueLockless< T >::pop(
    T   &data
    )
{
    auto read_index     = m_read.load( std::memory_order_relaxed ); 
    auto write_index    = m_write.load( std::memory_order_relaxed ); 

    if ( read_index == write_index ) 
    {
        // Ring is empty.

        return false;
    }

    // Get data.
    *data = m_data[ read_index ];

    // Update the read index.
    m_read.store( ( read_index + 1 ) % m_totalSize, std::memory_order_release );

    // Success.

    return true;
}

