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

// message_queue.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "message_queue.h"
#include "timer.h"

using namespace  morsamdesa;


namespace morsamdesa
{

extern C_log      log;

struct msg_status_entry
{
    eMessageStatus   status_e;
    const char *     status;
};

static msg_status_entry state_lookup[] =
{
    { msUnplayed,   "U"  },
    { msHeld,       "H"  },
    { msPlayed,     "P"  },
    { msInvalid,    NULL }
};

C_message_queue::C_message_queue()
{
    entries_max_ = 512;

    add_to_front_of_queue_ = false;
    last_add_time_.tv_sec  = 0;

    most_recent_item_.source   = fsNone;
    most_recent_item_.mnemonic = "QUE";
    most_recent_item_.data     = "None";
    most_recent_item_.discard  = true;
}

/** \brief Set queue parameters
*
*/
void
C_message_queue::initialise( unsigned int max_unheld )
{
    unheld_max_      = max_unheld;
}

/** \brief Add a message to the top of the queue
*
*  @param[in]  msg     Message to add
*  @param[in]  option  Message handling options ( e.g. discard after sending, do not hold on queue )
*/
void
C_message_queue::add( C_data_feed_entry & feed_item )
{
    set_queue_mode();

    C_message_entry *entry = new C_message_entry;

    entry->status         = msUnplayed;
    entry->feed_item      = feed_item;
    entry->feed_item.time = C_timer::current_time();
    
    qlock_.lock();

    // Since we're about to add a new message to the top of the queue, flag older messages as Held so they won't be
    // automatically sent. The exception to this is discardable messages, which should be played as soon as possible
    // then thrown away.
    // The number of messages which can be left unheld is controlled by unheld_max_. For example, if unheld_max_ was
    // set to 10, then up to ten 10 messages would be available for sending, and these would be sent out in succession.

    for ( unsigned int ii = 0, unheld_count = 0; ii < queue_.size(); ii++ )
    {
        if ( ( queue_[ ii ].status == msUnplayed ) && ( ! ( queue_[ ii ].feed_item.discard ) ) )
        {
            if ( ++unheld_count >= unheld_max_ )
            {
                queue_[ ii ].status = msHeld;
            }
        }
    }

    if ( add_to_front_of_queue_ )
    {
        queue_.push_front( *entry );
    }
    else
    {
        queue_.push_back( *entry );
    }

    qlock_.unlock();

    remove_old_messages();
}

bool
C_message_queue::empty()
{
    return ! got_unplayed_message();
}

/** \brief
*
*  @param[out]  msg Message
*/
void
C_message_queue::get_most_recent( C_data_feed_entry & feed_item )
{
    qlock_.lock();
    feed_item = most_recent_item_;
    qlock_.unlock();
}

/** \brief
*
*  @param[out]  msg Message
*/
bool
C_message_queue::get_next_unplayed( C_data_feed_entry & feed_item )
{
    return get_next( feed_item, msUnplayed );
}

/** \brief
*
*  @param[out]  msg Message
*/
bool
C_message_queue::get_next_held( C_data_feed_entry & feed_item )
{
    return get_next( feed_item, msHeld );
}

/** \brief Return the message nearest to the top of the queue which has the specified status
*
*  @param[out]  msg Message
*/
bool
C_message_queue::get_next( C_data_feed_entry & feed_item, eMessageStatus status )
{
    qlock_.lock();

    bool found = false;

    for ( unsigned int ii = 0; ii < queue_.size(); ii++ )
    {
        if ( queue_[ ii ].status == status )
        {
            feed_item = queue_[ ii ].feed_item;
            found = true;
            break;
        }
    }

    qlock_.unlock();

    if ( ! found )
    {
        feed_item.source   = fsNone;
        feed_item.mnemonic = "QUE";
        feed_item.data     = "None";
    }

    most_recent_item_ = feed_item;

    return found;
}

/** \brief Flag a message as having been sent
*
*   @param[in]  msg Message
*/
void
C_message_queue::mark_as_played( const C_data_feed_entry & feed_item )
{
    unsigned int queue_index = 0;

    qlock_.lock();

    if ( find( feed_item.data, queue_index ) )
    {
        if ( queue_[ queue_index ].feed_item.discard )
        {
            log_writeln_fmt( C_log::LL_INFO, "Discarding entry %u ('%s')", queue_index, feed_item.data.c_str() );

            // Delete message from queue
            queue_.erase( queue_.begin() + queue_index );
        }
        else
        {
            // Flag as played
            queue_[ queue_index ].status = msPlayed;
            log_writeln_fmt( C_log::LL_VERBOSE_2, "Marked entry %u as played ('%s')", queue_index, feed_item.data.c_str() );
        }
    }

    qlock_.unlock();
}

/** \brief Check if queue contains messages waiting to be sent
*
*  @return true if message waiting
*/
bool
C_message_queue::got_unplayed_message()
{
    qlock_.lock();

    for ( unsigned int ii = 0; ii < queue_.size(); ii++ )
    {
        if ( queue_[ ii ].status == msUnplayed )
        {
            qlock_.unlock();
            return true;
        }
    }

    qlock_.unlock();

    return false;
}

/** \brief Find a message in the queue
*
*   @param[in]   msg   Message
*   @param[out]  index Index of queue entry
*/
bool
C_message_queue::find( const string & msg, unsigned int & index )
{
    for ( unsigned int ii = 0; ii < queue_.size(); ii++ )
    {
        if ( queue_[ ii ].feed_item.data == msg )
        {
            index = ii;
            return true;
        }
    }

    return false;
}

/** \brief Remove messages older than a certain age
*/
void
C_message_queue::remove_old_messages()
{
    remove_messages( false );
}

/** \brief Remove all messages from queue
*/
void
C_message_queue::discard_all_messages()
{
    remove_messages( true );
}

/** \brief Remove messages from the end of the queue to bring the number of queue entries back to the required size
*/
void
C_message_queue::remove_messages( bool all )
{
    qlock_.lock();

    for ( unsigned int ii = queue_.size(); ii > 0; ii-- )
    {
        if ( all || message_too_old( queue_[ ii - 1 ] ) )
        {
            queue_.pop_back();
        }
        else
        {
            // We're not deleting all messages, and there are no more old messages to remove
            break;
        }
    }

    qlock_.unlock();
}

bool
C_message_queue::message_too_old( const C_message_entry & message_entry )
{
    return C_timer::elapsed_sec( message_entry.feed_item.time ) > C_timer::DAY_SECS;
}

// For the initial spate of messages, add them to the back of the queue. From then on, new messages
// are added to the front of the queue.
void
C_message_queue::set_queue_mode()
{
    if ( ! add_to_front_of_queue_ )
    {
        if ( last_add_time_.tv_sec == 0 )
        {
            last_add_time_ = C_timer::current_time();
        }
        else if ( C_timer::elapsed_sec( last_add_time_ ) > 10 )
        {
            add_to_front_of_queue_ = true;
        }
    }
}

const char *
C_message_queue::to_string( eMessageStatus status_e )
{
    for ( msg_status_entry *p = state_lookup; p->status; p++ )
    {
        if ( p->status_e == status_e )
        {
            return p->status;
        }
    }

    return "?";
}

/** \brief Show queue contents

    NB: For diagnostic purposes only. Calling this method introduces a delay into the main
        background task loop that will cause an audible hitch in background noise.
*/
void
C_message_queue::display_queue( const char * action )
{
    if ( log.log_level() >= C_log::LL_INFO )
    {
        log_writeln_fmt( C_log::LL_INFO, "Message queue: %s", action );

        unsigned int queue_size = queue_.size();

        const unsigned int entries_per_line = 3;

        string line = "";

        for ( unsigned int ii = 0; ii < queue_size; )
        {
            for ( unsigned int jj = 0; ( jj < entries_per_line ) && ( ii < queue_size ); jj++, ii++ )
            {
                char entry[ 50 ];

                snprintf( entry, sizeof( entry ), "{%3d}%-1.1s [%-3.3s][%-7.7s]%-12.12s... "
                                                , ii + 1
                                                , to_string( queue_[ ii ].status )
                                                , queue_[ ii ].feed_item.mnemonic.c_str()
                                                , C_timer::elapsed_str( queue_[ ii ].feed_item.time ).c_str()
                                                , queue_[ ii ].feed_item.data.c_str() );
                line += entry;
            }

            if ( line.length() > 0 )
            {
                log_writeln_fmt( C_log::LL_INFO, "%s", line.c_str() );
                line = "";
            }
        }
    }
}

}
