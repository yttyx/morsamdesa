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

/*! \file datafeed.cpp
*/

#include <regex.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <ctime>
#include <string>
#include <vector>

#include "datafeed.h"
#include "log.h"
#include "random.h"

using namespace  morsamdesa;


namespace morsamdesa
{

extern C_log log;


void C_data_feed_entry::operator=( const C_data_feed_entry & rhs )
{
    source   = rhs.source;
    mnemonic = rhs.mnemonic;
    data     = rhs.data;
    discard  = rhs.discard;
}

C_data_feed::C_data_feed()
    : abort_( false )
{
}

C_data_feeds::C_data_feeds()
{
    feed_idx_prev_ = 0xFF;
}

C_data_feeds::~C_data_feeds()
{
}

void
C_data_feeds::add( C_data_feed * feed )
{
    data_feeds_.push_back( feed );
}

bool
C_data_feeds::start()
{
    // Start up data sources
    for ( unsigned int ii = 0; ii < data_feeds_.size(); ii++ )
    {
        if ( ! data_feeds_[ ii ]->start() )
        {
            log_writeln( C_log::LL_ERROR, "Error: failed to start a data feed." );
            return false;
        }
    }
    return true;
}

bool
C_data_feeds::read( C_data_feed_entry & feed_item )
{
    C_random random( data_feeds_.size() );

    int feeds_ready = 0;

    // Check for data from each feed and remove a feed's index from the random list if it has no data ready
    for ( unsigned int ii = 0; ii < data_feeds_.size(); ii++ )
    {
        if (  data_feeds_[ ii ]->data_ready() )
        {
            feeds_ready++;
        }
        else
        {
            random.remove( ii );
        }
    }

    // Avoid fetching from the same feed in succession if possible
    if ( feeds_ready > 1 )
    {
        random.remove( feed_idx_prev_ );
    }

    if ( feeds_ready > 0 )
    {
        // Choose a (semi-) random feed to read
        unsigned int feed_idx = random.next();

        feed_idx_prev_ = feed_idx;

        if ( data_feeds_[ feed_idx ]->read( feed_item ) )
        {
            return true;
        }
        else
        {
            log_writeln_fmt( C_log::LL_ERROR, "C_data_feeds::read(): no data when data expected: feeds_ready = %d;  feed_idx = %u ", feeds_ready, feed_idx );
        }
    }

    return false;
}

bool
C_data_feeds::all_read()
{
    unsigned int feed_done_count = 0;

    // Check data feeds to see if all data is exhausted
    for ( unsigned int ii = 0; ii < data_feeds_.size(); ii++ )
    {
        if ( data_feeds_[ ii ]->no_more_data() )
        {
            feed_done_count++;
        }
    }

    return feed_done_count == data_feeds_.size();
}

void
C_data_feeds::stop()
{
    // Close down data sources
    for ( unsigned int ii = 0; ii < data_feeds_.size(); ii++ )
    {
        data_feeds_[ ii ]->stop();
    }

    // Free resources
    while ( data_feeds_.size() )
    {
        C_data_feed *feed = data_feeds_.back();

        data_feeds_.pop_back();

        delete feed;
    }
}

}
