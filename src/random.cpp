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

// random.cpp

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "log.h"
#include "random.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;

C_random::C_random( int max )
{
    max_        = max;
    values_     = new bool[ max ];

    init_values();
}

C_random::~C_random()
{
    delete [] values_;
}

void C_random::init()
{
    // Initialize random seed
    srand( time( NULL ) );
}

// Random value between 0 and ( max_ - 1 ), omitting any values which have been removed
unsigned int
C_random::next()
{
    return next_internal( false );
}

// Return a random number in the range 0 - (max - 1) with the proviso that a given number should not be repeated
// until all numbers in the range have been used up.
unsigned int
C_random::next_unique()
{
    return next_internal( true );
}

unsigned int
C_random::next_internal( int unique )
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
            if ( unique )
            {
                remove( val );
            }
            
            return ( unsigned int ) val;
        }
    }
}

// Remove val from the list of available numbers
void
C_random::remove( int val )
{
    if ( ( val >= 0 ) && ( val <= ( max_ - 1 ) ) )
    {
        values_[ val ] = true;
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

void
C_random::display_available()
{
    string active;

    for ( int ii = 0; ii < max_; ii++ )
    {
        active += values_[ ii ] ? " " : C_log::format_string( "%d", ii );
    }

    log_writeln_fmt( C_log::LL_INFO, "C_random, available values: %s", active.c_str() );
}

}
