/*
    Copyright (C) 2018  yttyx

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

// mutex.h

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

namespace morsamdesa
{

class C_mutex
{
public:

    C_mutex()
    {
        // Initialize to defaults
        pthread_mutex_init( &mutex_, NULL);
    }

    virtual
    ~C_mutex()
    {
        pthread_mutex_destroy( &mutex_ );
    }

    int
    lock()
    {
        return pthread_mutex_lock( &mutex_ );
    }

    int
    try_lock()
    {
        return pthread_mutex_trylock( &mutex_ );
    }

    int
    unlock()
    {
        return pthread_mutex_unlock( &mutex_ );
    }

private:

    pthread_mutex_t     mutex_;

};

}

#endif  // MUTEX_H
