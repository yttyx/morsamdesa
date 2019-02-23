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

// night_mode.h

#ifndef night_mode_H
#define night_mode_H

#include <time.h>


using namespace std;


namespace morsamdesa
{

enum eNightModeStatus
{
    nmEntered,
    nmLeft,
    nmNoChange
};


class C_night_mode
{
public:

    C_night_mode();
    ~C_night_mode();

    void
    initialise( const string & start_time, const string & end_time );

    bool
    active();

    eNightModeStatus
    status();

private:

    void
    set_time( const string & time_str, time_t & tt );

    void
    display_start_and_end_times();

    string &
    format_datetime( time_t tt );

private:

    bool    night_mode_active_;

    time_t  start_time_;
    time_t  end_time_;
};

}

#endif // night_mode_H
