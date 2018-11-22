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

// audio_output_pulseaudio.cpp:

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "misc.h"
#include "audio_output_pulseaudio.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;


C_audio_output_pulseaudio::C_audio_output_pulseaudio()
{
    pa_handle_ = NULL;
}

C_audio_output_pulseaudio::~C_audio_output_pulseaudio()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_audio_output_pulseaudio destructor" );

    if ( ! pa_handle_ )
    {
        int error = 0;
        
        // Wait until all data already written has been played by the Pulseaudio daemon
        pa_simple_drain( pa_handle_, &error );

        if ( error )
        {
            log_writeln_fmt( C_log::LL_ERROR, "C_pulseaudio::pa_simple_drain():  error %s", pa_strerror( error ) );
        }
        
        pa_simple_free( pa_handle_ );
    }
}

bool
C_audio_output_pulseaudio::initialise()
{
    bool worked = C_audio_output::initialise();

    log_writeln_fmt( C_log::LL_VERBOSE_2, "C_audio_output_pulseaudio::initialise() : 1, %s", worked ? "true" : "false" );

    // Sample format to use
    static pa_sample_spec sample_spec =
    {
        .format   = PA_SAMPLE_S16LE,
        .rate     = (unsigned int ) SAMPLE_RATE,
        .channels = 1
    };

    int error = 0;

    pa_handle_ = pa_simple_new( NULL,
                                "morsamdesa",
                                PA_STREAM_PLAYBACK,
                                NULL,
                                "playback",
                                &sample_spec,
                                NULL,
                                NULL,
                                &error );
    if ( ! pa_handle_ )
    {
        log_writeln_fmt( C_log::LL_ERROR, "pa_simple_new() failed: %s", pa_strerror( error ) );
    }

    return pa_handle_ != NULL;
}

/*
 * Send sound data to PulseAudio
 */
void
C_audio_output_pulseaudio::send( bool all )
{
    int error = 0;

    pa_simple_write( pa_handle_, output_buffer_, output_buffer_size_ * sizeof( short int ), &error );

    if ( error )
    {
        log_writeln_fmt( C_log::LL_ERROR, "C_pulseaudio::pa_simple_write():  error %s", pa_strerror( error ) );
    }

    if ( all )
    {
        if ( error )
        {
            log_writeln_fmt( C_log::LL_ERROR, "C_pulseaudio::pa_simple_drain():  error %s", pa_strerror( error ) );
        }
    }
}

}
