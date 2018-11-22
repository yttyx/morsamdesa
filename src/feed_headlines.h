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

// feed_headlines.h

#ifndef feed_headlines_H
#define feed_headlines_H

#include <queue>
#include <string>

#include "headlines.h"


using namespace std;

namespace morsamdesa
{

class C_feed_headlines : public C_headlines
{
public:

    C_feed_headlines( const string & url, const vector <string> & filters, unsigned int period_min, unsigned int period_max );
    virtual ~C_feed_headlines();

    virtual bool
    start();

    virtual void
    stop();

    virtual bool
    data_ready();

    virtual bool
    read( string & str );

    virtual void
    process_headlines();

private:

    queue< string > feed_;
    C_mutex         feed_lock_;

};

}

#endif // feed_headlines_H
