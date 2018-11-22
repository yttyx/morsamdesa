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

// feed_headlines.cpp
//

#include <algorithm>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "feed_headlines.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;


C_feed_headlines::C_feed_headlines( const string & url, const vector <string> & filters, unsigned int period_min, unsigned int period_max )
    : C_headlines( url, filters, period_min, period_max )
{
}

C_feed_headlines::~C_feed_headlines()
{
}

bool
C_feed_headlines::start()
{
    if ( ! filter_.add_filters( filter_strings_ ) )
    {
        return false;
    }
    
    // Start up the background headline polling thread
    C_headlines::start();
    
    return true;
}

void
C_feed_headlines::stop()
{
    C_headlines::stop();

    thread_await_exit();
}

bool
C_feed_headlines::data_ready()
{
    feed_lock_.lock();
    
    bool got_data = ( feed_.size() > 0 );
    
    feed_lock_.unlock();

    return got_data;
}

bool
C_feed_headlines::read( string & str )
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
C_feed_headlines::process_headlines()
{
    hlock_.lock();

    unsigned int headlines_prev = ( headlines_curr_ == 1 ) ? 0 : 1;

    if ( headlines_[ headlines_prev ].size()  == 0 )
    {
        // First headlines read. Use up to the first 16 entries as the feed.
        unsigned int entries = min( ( int ) headlines_[ headlines_curr_ ].size(), 16 );

        for ( unsigned int ii = 0; ii < entries; ii++ )
        {
            feed_lock_.lock();
            feed_.push( headlines_[ headlines_curr_ ][ ii ] );
            feed_lock_.unlock();
        }
    }
    else
    {
        // Compare the current and previous headline arrays. Just use new entries
        // (absent from previous array; present in current array)

        for ( unsigned int headline_curr_idx = 0; headline_curr_idx < headlines_[ headlines_curr_ ].size(); headline_curr_idx++ )
        {
            bool found = false;

            for ( unsigned int headline_prev_idx = 0; headline_prev_idx < headlines_[ headlines_prev ].size(); headline_prev_idx++ )
            {
                if ( headlines_[ headlines_curr_ ][ headline_curr_idx ] == headlines_[ headlines_prev ][ headline_prev_idx ] )
                {
                    found = true;
                    break;
                }
            }
            
            if ( ! found )
            {
                feed_lock_.lock();
                feed_.push( headlines_[ headlines_curr_ ][ headline_curr_idx ] );
                feed_lock_.unlock();
            }
        }
    }

    hlock_.unlock();
}

}
