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

// pulse_to_morse.cpp

#include <assert.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "log.h"
#include "pulse_to_morse.h"

using namespace  morsamdesa;

namespace morsamdesa
{
extern C_log log;


C_pulse_to_morse::C_pulse_to_morse( unsigned int dot_duration )
{
    duration_curr_ = 0;
    duration_dot_  = dot_duration;
}

void
C_pulse_to_morse::add( unsigned int duration )
{
    if ( duration_curr_ < DURATIONS_MAX )
    {
        durations_[ duration_curr_++ ] = duration;
    }
}

void
C_pulse_to_morse::reset()
{
    duration_curr_ = 0;
}

string
C_pulse_to_morse::read()
{
    // Use the array of pulse durations to build a Morse character in dots and dashes
    morse_ = "";

    for ( unsigned int ii = 0; ii < duration_curr_; ii++ )
    {
        morse_ += ( durations_[ ii ] > ( duration_dot_ * 2 ) ? "-" : "." );
    }

    return morse_;
}

}
