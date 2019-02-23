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

// datasource.h

#ifndef datasource_H
#define datasource_H

#include <string>
#include <vector>

#include "feed_source.h"
#include "timer.h"


using namespace std;

namespace morsamdesa
{

struct C_data_feed_entry
{
public:

    C_data_feed_entry() {}
    C_data_feed_entry( string & src, string & text ) { mnemonic = src; data = text; discard = false; }
    ~C_data_feed_entry() {}

    void
    operator=( const C_data_feed_entry & rhs );

public:

    eFeedSource source;
    string      mnemonic;
    string      data;
    timespec    time;
    bool        discard;
};


class C_data_feed
{
public:

    C_data_feed();

    virtual ~C_data_feed(){}

    virtual bool
    start(){ return false; }

    virtual void
    stop(){ abort_ = true; }

    virtual bool
    data_ready(){ return false; }

    virtual bool
    no_more_data(){ return false; }

    virtual bool
    read( C_data_feed_entry & feed_item ) { return false; }

protected:

    bool abort_;

};


class C_data_feeds
{
public:

    C_data_feeds();
    ~C_data_feeds();

    void
    add( C_data_feed * feed );

    bool
    start();

    void
    stop();

    bool
    read( C_data_feed_entry & feed_item );

    virtual bool
    all_read();

protected:

    vector< C_data_feed * > data_feeds_;
    unsigned int            feed_idx_prev_;
};

}

#endif // datasource_H
