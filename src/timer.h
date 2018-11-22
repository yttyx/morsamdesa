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

// timer.h

#ifndef timer_H
#define timer_H

using namespace std;


namespace morsamdesa
{

class C_timer
{
public:

    C_timer();
    ~C_timer() {}

    void
    set( long milliseconds );

    void
    start( long milliseconds );

    bool
    expired();

    string
    elapsed();

    bool active() { return active_; }

private:

    void
    diff( const timespec & start, const timespec & end, timespec & elapsed );

private:

    long            duration_;
    struct timespec start_time_;
    bool            active_;
};

}

#endif // timer_H
