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

// remote_control.h

#ifndef    remote_control_I
#define    remote_control_I

#include <poll.h>
#include <queue>

#include "command.h"
#include "remote_stub.h"
#include "thread.h"


namespace morsamdesa
{

class C_remote_control : public C_remote_stub, public C_thread
{

public:

    C_remote_control();
    ~C_remote_control();

    virtual bool
    initialise();

    virtual bool
    start();

    virtual void
    stop();

    virtual eCommand
    read();

private:

    void
    thread_handler();

private:

    int                 handle_;

    string              device_;

    queue< eCommand >   buffer_;
    C_mutex             buffer_lock_;

    struct pollfd       pollfd_;

    static const eCommand remote_table[];
};

}

#endif    // remote_control_I
