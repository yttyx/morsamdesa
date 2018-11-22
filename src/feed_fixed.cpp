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

// feed_fixed.cpp
//

#include <algorithm>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "misc.h"
#include "feed_fixed.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;


C_feed_fixed::~C_feed_fixed()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_feed_fixed destructor" );
}

bool
C_feed_fixed::start()
{
    return true;
}

bool
C_feed_fixed::data_ready()
{
    return true;
}

bool
C_feed_fixed::read( string & str )
{
    str = *message_;

    return true;
}

}
