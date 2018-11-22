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

// random.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "random.h"

using namespace  morsamdesa;

namespace morsamdesa
{

C_random::C_random( int max )
{
    max_        = max;
    values_     = new bool[ max ];

    init_values();
}

C_random::~C_random()
{
    if ( values_ )
    {
        delete [] values_;
    }
}

void C_random::init()
{
    // Initialize random seed
    srand( time( NULL ) );
}

unsigned int
C_random::random()
{
    // Random value between 0 and ( max_ - 1 )
    return rand() % max_;
}

// Return a random number in the range 0 - (max - 1) with the proviso that a given number should not be repeated
// until all numbers in the range have been used up.
unsigned int
C_random::not_in_last_n()
{
    if ( ! spare_slot() )
    {
       init_values();
    }

    while ( true )
    {
        // Random value between 0 and ( max_ - 1 )
        int val = rand() % max_;
        
        if ( ! values_[ val ] )
        {
            values_[ val ] = true;

            return ( unsigned int ) val;
        }
    }
}

void
C_random::init_values()
{
    for ( int ii = 0; ii < max_; ii++ )
    {
        values_[ ii ] = false;
    }
}

bool
C_random::spare_slot()
{
    for ( int ii = 0; ii < max_; ii++ )
    {
        if ( ! values_[ ii ] )
        {
            return true;
        }
    }

    return false;
}

}
