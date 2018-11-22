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

// silence.cpp

#include <assert.h>

#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"
#include "misc.h"
#include "silence.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;

C_silence::C_silence()
{
}

C_silence::~C_silence()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_silence destructor" );
}


void
C_silence::write()
{
    bool samples_exhausted = false;

    write( samples_exhausted );
}

/* /brief Add some silence
 *
 * /detail Supply of samples is based around supplying bursts of tick_samples samples or fewer
 *
 *         This method doesn't affect the output buffer at all: it effectively acts as a sample
 *         counter to implement the required period of silence.
 */
void
C_silence::write( bool & samples_exhausted )
{
    samples_exhausted = false;

    if ( active_ )
    {
        for ( ;; )
        {
            // output_->write() returns true if the output buffer is full *after* the sample is added to the buffer

            bool output_buffer_full = output()->write( 0 );     // 0 == silence

            if ( ++sample_curr_ >= samples_ )
            {
                samples_exhausted = true;
                active_           = false;
                break;
            }

            if ( output_buffer_full )
            {
                break;
            }
        }
    }
}

bool
C_silence::initialise( unsigned int samples,  C_audio_output * output )
{
    samples_ = samples;
    output_  = output;
    return true;
}

}
