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

// message_queue.h

#ifndef messagequeue_H
#define messagequeue_H

#include <deque>
#include <string>

#include "datafeed.h"
#include "mutex.h"
#include "timer.h"


using namespace std;

namespace morsamdesa
{

enum eMessageStatus
{
    msUnplayed,
    msHeld,
    msPlayed,
    msInvalid,
};

struct C_message_entry
{
public:

    C_message_entry() {}
    ~C_message_entry() {}

public:

    C_data_feed_entry   feed_item;
    eMessageStatus      status;
};

class C_message_queue
{
public:

    C_message_queue();
    ~C_message_queue() {}

    void
    initialise( unsigned int max_unheld );

    void
    add( C_data_feed_entry & feed_item );

    bool
    empty();

    bool
    get_next_unplayed( C_data_feed_entry & feed_item );

    bool
    get_next_held( C_data_feed_entry & feed_item );

    bool
    got_unplayed_message();

    void
    mark_as_played( const C_data_feed_entry & feed_item );

    void
    get_most_recent( C_data_feed_entry & feed_item );

    void
    discard_all_messages();

    void
    display_queue( const char * action );

private:

    bool
    get_next( C_data_feed_entry & feed_item, eMessageStatus status );

    bool
    find( const string & msg, unsigned int & index );

    void
    remove_old_messages();

    void
    remove_messages( bool all );

    bool
    message_too_old( const C_message_entry & message_entry );

    void
    set_queue_mode();

    const char *
    to_string( eMessageStatus status_e );

private:

    deque< C_message_entry > queue_;

    C_data_feed_entry      most_recent_item_;
    C_mutex                qlock_;

    timespec               last_add_time_;

    unsigned int           entries_max_;
    unsigned int           unheld_max_;
    bool                   add_to_front_of_queue_;
};

}

#endif // messagequeue_H
