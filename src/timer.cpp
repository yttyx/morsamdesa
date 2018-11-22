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

// timer.cpp

#include <assert.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "log.h"
#include "timer.h"

using namespace  morsamdesa;

namespace morsamdesa
{
extern C_log      log;

C_timer::C_timer()
{
    duration_ = 0;
    active_   = false;
}

void
C_timer::start( long milliseconds )
{
    duration_ = milliseconds;
    active_   = true;

    clock_gettime( CLOCK_REALTIME, &start_time_ );
}

bool
C_timer::expired()
{
    struct timespec curr_time;
    struct timespec elapsed_time;

    clock_gettime( CLOCK_REALTIME, &curr_time );

    diff( start_time_, curr_time, elapsed_time );

    long elapsed_ms = ( elapsed_time.tv_sec * 1000 ) + ( elapsed_time.tv_nsec / 1000000 );

    bool expired = elapsed_ms >= duration_;

    if ( expired )
    {
        active_ = false;
    }

    return expired;
}

string
C_timer::elapsed()
{
    struct timespec curr_time;
    struct timespec elapsed_time;

    clock_gettime( CLOCK_REALTIME, &curr_time );

    diff( start_time_, curr_time, elapsed_time );

    return C_log::format_string( "%ld.%ld", elapsed_time.tv_sec, elapsed_time.tv_nsec / 1000000 );
}

void
C_timer::diff( const timespec & start, const timespec & end, timespec & elapsed )
{
    if ( ( end.tv_nsec - start.tv_nsec ) < 0 )
    {
        elapsed.tv_sec  = end.tv_sec-start.tv_sec - 1;
        elapsed.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        elapsed.tv_sec  = end.tv_sec - start.tv_sec;
        elapsed.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
}

}
