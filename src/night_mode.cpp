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

// night_mode.cpp

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>

#include "log.h"
#include "night_mode.h"

using namespace  morsamdesa;

namespace morsamdesa
{
extern C_log      log;

const time_t SECONDS_PER_DAY = 24 * 60 * 60;


C_night_mode::C_night_mode()
{
    night_mode_active_ = false;
    start_time_        = 0;
    end_time_          = 0;
}

C_night_mode::~C_night_mode()
{
}

void
C_night_mode::initialise( const string & start_time, const string & end_time )
{
    if ( ( start_time.compare( "00:00" ) == 0 ) &&
         ( end_time.compare( "00:00" ) == 0 ) )
    {
        log_writeln( C_log::LL_INFO, "Night mode: Disabled" );
        return;
    }
    
    // Combine today's date with the supplied start time and end time
    set_time( start_time, start_time_ );
    set_time( end_time, end_time_ );

    time_t now = time( NULL );

    if ( end_time_ < start_time_ )
    {
        if ( ( now < end_time_ ) || ( now > start_time_ ) )
        {
            night_mode_active_ = true;
        }
    }
    else
    {
        if ( ( start_time_ <= now ) && ( now <= end_time_ ) )
        {
            night_mode_active_ = true;
        }
    }

    // Set next trigger time

    if ( start_time_ < now )
    {
        start_time_ += SECONDS_PER_DAY;
    }
    if ( end_time_ < now )
    {
        end_time_ += SECONDS_PER_DAY;
    }
}

bool
C_night_mode::active()
{
    return night_mode_active_;
}

eNightModeStatus
C_night_mode::status()
{
    if ( ( start_time_ > 0 ) || ( end_time_ > 0 ) )
    {
        time_t tt_now = time( NULL );

        if ( tt_now >= start_time_ )
        {
            start_time_ += SECONDS_PER_DAY;
            night_mode_active_ = true;
            return nmEntered;
        }

        if ( tt_now >= end_time_ )
        {
            end_time_ += SECONDS_PER_DAY;
            night_mode_active_ = false;
            return nmLeft;
        }
    }

    return nmNoChange;
}

// /param  time is in the format HH:MM
void
C_night_mode::set_time( const string & time_str, time_t & tt )
{
    time_t    tt_now  = time( NULL );
    struct tm *tm_now = localtime( &tt_now );
    struct tm tm_time = *tm_now;

    tm_time.tm_hour = atoi( time_str.substr( 0, 2 ).c_str() );
    tm_time.tm_min  = atoi( time_str.substr( 3, 2 ).c_str() );
    tm_time.tm_sec  = 0;

    tt = mktime( &tm_time );
}

void
C_night_mode::display_start_and_end_times()
{
    log_writeln( C_log::LL_INFO, "C_night_mode::display_start_and_end_times()" );
    log_writeln_fmt( C_log::LL_INFO, "  start_time_ = %s", format_datetime( start_time_ ).c_str() );
    log_writeln_fmt( C_log::LL_INFO, "  end_time_   = %s", format_datetime( end_time_ ).c_str() );
}

string &
C_night_mode::format_datetime( time_t tt )
{
    static string datetime;

    char buff[ 20 ];

    strftime( buff, 20, "%Y-%m-%d %H:%M:%S", localtime( &tt ) );

    datetime = buff;

    return datetime;
}

}
