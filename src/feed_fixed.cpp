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

// feed_fixed.cpp
//

#include <algorithm>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed_fixed.h"
#include "log.h"
#include "misc.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;


C_feed_fixed::C_feed_fixed( const string & message )
{
    feed_entry_.source   = fsFixed;
    feed_entry_.mnemonic = "FXD";
    feed_entry_.data     = message;
    feed_entry_.discard  = true;
}

C_feed_fixed::~C_feed_fixed()
{
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
C_feed_fixed::read( C_data_feed_entry & feed_entry )
{
    feed_entry = feed_entry_;

    return true;
}

}
