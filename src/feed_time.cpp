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

// feed_time.cpp
//

#include <algorithm>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "misc.h"
#include "feed_time.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;


C_feed_time::C_feed_time( unsigned int period )
    : period_( period)
{
    memset( &tm_prev_, 0, sizeof( tm_prev_ ) );
}

C_feed_time::~C_feed_time()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_feed_time destructor" );
}

bool
C_feed_time::start()
{
    return thread_start();
}

void
C_feed_time::stop()
{
    C_data_feed::stop();

    thread_await_exit();
}

bool
C_feed_time::data_ready()
{
    feed_lock_.lock();
    
    bool got_data = ( feed_.size() > 0 );
    
    feed_lock_.unlock();

    return got_data;
}

bool
C_feed_time::read( string & str )
{
    bool got_data = false;

    feed_lock_.lock();

    got_data = ( feed_.size() > 0 );

    if ( got_data )
    {
        str = feed_.front();
        feed_.pop();
    }

    feed_lock_.unlock();

    return got_data;
}

// -----------------------------------------------------------------------------------
// Background thread code
// -----------------------------------------------------------------------------------

void
C_feed_time::thread_handler()
{
    while ( ! abort_  )
    {
        // Get time
        tm tm_now;

        time_t  now = time( 0 );
        tm_now      = *localtime( &now );

        if ( ( tm_now.tm_hour != tm_prev_.tm_hour ) || ( tm_now.tm_min != tm_prev_.tm_min ) )
        {
            // Check if time has reached the specified period
            if ( ( tm_now.tm_min % period_ ) == 0 )
            {
                // It has - format the time and add it to the output queue
                feed_lock_.lock();
                
                string time = "T: ";

                time += format_time_hh_mm( tm_now );
                
                feed_.push( time );
                feed_lock_.unlock();
            }

            tm_prev_ = tm_now;
        }
        
        // If not, sleep for 9 seconds
        delay( 9000 );
    }

    // Discard any queued messages
    feed_lock_.lock();

    while ( feed_.size() > 0 )
    {
        feed_.pop();
    }

    feed_lock_.unlock();
}

}
