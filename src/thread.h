/*
    Copyright (C) 2018  yttyx. This file is part of morsamdesa.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

// thread.h

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

namespace morsamdesa
{

class C_thread
{
public:
    C_thread() {}
    virtual ~C_thread() {}

    // Returns true if the thread was successfully started, false if there was an error starting the thread
    bool thread_start()
    {
        return ( pthread_create( &thread_, NULL, thread_entry_func, this ) == 0 );
    }

    // Will not return until the internal thread has exited
    void thread_await_exit()
    {
        (void) pthread_join( thread_, NULL );
    }

protected:
    // Implement this method in your subclass with the code you want your thread to run
    virtual void thread_handler() = 0;

private:
    static void *thread_entry_func( void * me )
    {
        ( ( C_thread *) me )->thread_handler();
        return NULL;
    }

private:
    pthread_t thread_;

};

}

#endif  // THREAD_H
