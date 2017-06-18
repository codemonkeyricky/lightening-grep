#pragma once

template <class T>
class iQueue
{
public:
    iQueue() { } ;
    virtual ~iQueue() { };

    virtual bool push( T & )  = 0;
    virtual bool pop( T & ) = 0;
    virtual uint32_t size() = 0;
};
