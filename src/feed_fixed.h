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

// feed_fixed.h

#ifndef feed_fixed_H
#define feed_fixed_H

#include <queue>
#include <string>

#include "datafeed.h"
#include "thread.h"


using namespace std;

namespace morsamdesa
{

class C_feed_fixed : public C_data_feed
{
public:

    C_feed_fixed( const string * message ){ message_ = message; }
    ~C_feed_fixed();

    virtual bool
    start();

    virtual bool
    data_ready();

    virtual bool
    discard(){ return true; }

    virtual bool
    read( string & str );

private:

    const string * message_;
};

}

#endif // feed_fixed_H
