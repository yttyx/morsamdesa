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

// feed_wordlist.h

#ifndef feed_wordlist_H
#define feed_wordlist_H

#include <queue>
#include <string>

#include "datafeed.h"
#include "random.h"
#include "thread.h"


using namespace std;

namespace morsamdesa
{

class C_feed_wordlist : public C_data_feed
{
public:

    C_feed_wordlist( const string & wordlist_file, bool repeat, bool random_order );
    virtual ~C_feed_wordlist();

    virtual bool
    start();

    virtual bool
    data_ready();

    virtual bool
    no_more_data() { return no_more_data_; }

    virtual bool
    discard(){ return true; }

    virtual bool
    read( string & str );

private:

    bool
    populate_wordlist( const string & filename );

private:

    const string    wordlist_file_;
    bool            repeat_;
    bool            random_order_;
    
    bool            no_more_data_;

    unsigned int    word_max_;
    unsigned int    word_curr_;
    unsigned int    word_count_;

    C_random            * random_number_;
    vector< string >    wordlist_;

};

}

#endif // feed_wordlist_H
