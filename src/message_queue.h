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

// message_queue.h

#ifndef messagequeue_H
#define messagequeue_H

#include <deque>
#include <string>

#include "mutex.h"


using namespace std;

namespace morsamdesa
{

enum eMessageOption
{
    moNone,
    moDiscard,
    moDoNotHold
};

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

    string         message;
    eMessageStatus status;
    eMessageOption option;
};

class C_message_queue
{
public:

    C_message_queue( unsigned int max_entries, unsigned int max_unheld, unsigned int max_discardable );
    ~C_message_queue() {}

    void
    add( const string & msg, bool discard );

    bool
    full();

    bool
    empty();

    bool
    get_next_unplayed( string & msg );

    bool
    get_next_held( string & msg );

    bool
    got_unplayed_message();

    void
    mark_as_played( const string & msg );

    void
    get_most_recent( string & msg );

    void
    discard_all_messages();

    void
    display_queue();

private:

    bool
    get_next( string & msg, eMessageStatus status );

    bool
    find( const string & msg, unsigned int & index );

    void
    remove_old_messages();

    void
    remove_messages( unsigned int retained_messages );

    const char *
    to_string( eMessageStatus status_e );

private:

    deque< C_message_entry > queue_;

    string                 most_recent_message_;

    C_mutex                qlock_;

    unsigned int           entries_max_;
    unsigned int           unheld_max_;
    unsigned int           discardable_max_;
    unsigned int           discardable_curr_;
};

}

#endif // messagequeue_H
