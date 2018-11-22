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

// message_queue.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "message_queue.h"

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
    { msUnplayed,   "Unplayed"  },
    { msHeld,       "Held"      },
    { msPlayed,     "Played"    },
    { msInvalid,    NULL        }
};

C_message_queue::C_message_queue( unsigned int max_entries, unsigned int max_unheld, unsigned int max_discardable )
    :   entries_max_( max_entries ),
        unheld_max_( max_unheld ),
        discardable_max_( max_discardable )
{
    discardable_curr_    = 0;
    most_recent_message_ = "None";
}

/** \brief Add a message to the top of the queue
*
*  @param[in]  msg     Message to add
*  @param[in]  option  Message handling options ( e.g. discard after sending, do not hold on queue )
*/
void
C_message_queue::add( const string & msg, bool discard )
{
    C_message_entry *entry = new C_message_entry;

    entry->status  = msUnplayed;
    entry->message = msg;
    entry->option  = discard ? moDiscard : moNone;

    qlock_.lock();

    // Since we're about to add a new message to the top of the queue, flag older messages as Held so they won't be
    // automatically sent. The exception to this is discardable messages, which should be played as soon as possible
    // then thrown away.
    // The number of messages which can be left unheld is controlled by unheld_max_. For example, if unheld_max_ was
    // set to 10, then up to ten 10 messages would be available for sending, and these would be sent out in succession.

    for ( unsigned int ii = 0, unheld_count = 0; ii < queue_.size(); ii++ )
    {
        if ( ( queue_[ ii ].status == msUnplayed ) && ( ! ( queue_[ ii ].option == moDiscard ) ) )
        {
            if ( ++unheld_count >= unheld_max_ )
            {
                queue_[ ii ].status = msHeld;
            }
        }
    }

    queue_.push_front( *entry );

    if ( discard )
    {
        discardable_curr_++;
    }

    qlock_.unlock();

    remove_old_messages();
}

bool
C_message_queue::full()
{
    bool full = false;

    qlock_.lock();
    full = discardable_curr_ >= discardable_max_;
    qlock_.unlock();

    return full;
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
C_message_queue::get_most_recent( string & msg)
{
    qlock_.lock();
    msg = most_recent_message_;
    qlock_.unlock();
}

/** \brief
*
*  @param[out]  msg Message
*/
bool
C_message_queue::get_next_unplayed( string & msg )
{
    return get_next( msg, msUnplayed );
}

/** \brief
*
*  @param[out]  msg Message
*/
bool
C_message_queue::get_next_held( string & msg )
{
    return get_next( msg, msHeld );
}

/** \brief Return the message nearest to the top of the queue which has the specified status
*
*  @param[out]  msg Message
*/
bool
C_message_queue::get_next( string & msg, eMessageStatus status )
{
    qlock_.lock();

    bool found = false;

    for ( unsigned int ii = 0; ii < queue_.size(); ii++ )
    {
        if ( queue_[ ii ].status == status )
        {
            msg = queue_[ ii ].message;
            found = true;
            break;
        }
    }

    qlock_.unlock();

    if ( ! found )
    {
        msg = "None";
    }

    most_recent_message_ = msg;

    return found;
}

/** \brief Flag a message as having been sent
*
*   @param[in]  msg Message
*/
void
C_message_queue::mark_as_played( const string & msg )
{
    unsigned int queue_index = 0;

    qlock_.lock();

    // The message may have dropped off the end of the queue if more new messages have arrived while the
    // message was being sent - but if the message does exist, flag it as having been played.
    if ( find( msg, queue_index ) )
    {
        if ( queue_[ queue_index ].option  == moDiscard )
        {
            log_writeln_fmt( C_log::LL_VERBOSE_2, "Discarding entry %u ('%s')", queue_index, msg.c_str() );

            // Delete message from queue
            queue_.erase( queue_.begin() + queue_index );

            discardable_curr_--;
        }
        else
        {
            // Flag as played
            queue_[ queue_index ].status = msPlayed;
            log_writeln_fmt( C_log::LL_VERBOSE_2, "Marked entry %u as played ('%s')", queue_index, msg.c_str() );
        }
    }

    qlock_.unlock();

    display_queue();
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
        if ( queue_[ ii ].message == msg )
        {
            index = ii;
            return true;
        }
    }

    return false;
}

/** \brief Remove messages to bring the number of queue entries back to the message queue's maximum required size
*/
void
C_message_queue::remove_old_messages()
{
    remove_messages( entries_max_ );
}

/** \brief Remove all messages from queue
*/
void
C_message_queue::discard_all_messages()
{
    remove_messages( 0 );
}

/** \brief Remove messages from the end of the queue to bring the number of queue entries back to the required size
*/
void
C_message_queue::remove_messages( unsigned int retained_messages )
{
    while ( true )
    {
        qlock_.lock();

        if ( queue_.size() > retained_messages )
        {
            queue_.pop_back();
            qlock_.unlock();
        }
        else
        {
            qlock_.unlock();
            break;
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

/** \brief Show queue contents (diagnostic)
*/
void
C_message_queue::display_queue()
{
    if ( log.log_level() >= C_log::LL_VERBOSE_3 )
    {
        log_writeln( C_log::LL_INFO, "" );
        log_writeln( C_log::LL_INFO, "Status    Message" );
        log_writeln( C_log::LL_INFO, "--------  ------------------------------------------------------------------------" );

        for ( unsigned int ii = 0; ii < queue_.size(); ii++ )
        {
            log_writeln_fmt( C_log::LL_INFO, "%-10.10s%-s", to_string( queue_[ ii ].status ), queue_[ ii ].message.c_str() );
        }

        log_writeln( C_log::LL_INFO, "---------------------------------------------------------------------------------------" );
        log_writeln( C_log::LL_INFO, "" );
    }
}

}
