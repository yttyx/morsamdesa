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

// feed_time.h

#ifndef feed_time_H
#define feed_time_H

#include <queue>
#include <string>

#include "datafeed.h"
#include "thread.h"


using namespace std;

namespace morsamdesa
{

class C_feed_time : public C_data_feed, C_thread
{
public:

    C_feed_time( unsigned int period );
    ~C_feed_time();

    virtual bool
    start();

    virtual void
    stop();

    virtual bool
    data_ready();

    virtual bool
    read( C_data_feed_entry & feed_entry );

private:

    void
    thread_handler();

private:

    queue< string > feed_;
    C_mutex         feed_lock_;

    tm              tm_prev_;
    unsigned int    period_;
};

}

#endif // feed_time_H
