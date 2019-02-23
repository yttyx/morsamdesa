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

// noise_file.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "noise_file.h"


using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;


C_noise_file::C_noise_file( const string & filename, eSoundMode mode )
    : C_sound_file( "noise", filename, mode, 0, 10.0 )
{
    bg_noise_fade_rising_edge_  = NULL;
    bg_noise_fade_falling_edge_ = NULL;
    state_                      = nsUnchanging;
    bg_noise_edge_curr_         = 0;
    bg_noise_level_current_     = 0.0;
}

C_noise_file::~C_noise_file()
{
    delete [] bg_noise_fade_rising_edge_;
    delete [] bg_noise_fade_falling_edge_;
}

void
C_noise_file::trigger( eNoiseState state )
{
    state_ = state;
}

/* /brief Add some sound samples to an output buffer
 *
 * /detail Supply of samples is based around supplying bursts of tick_samples samples or fewer
 *
 */
void
C_noise_file::write()
{
    if ( cfg.d().bg_noise )
    {
        for ( ;; )
        {
            if ( cfg.d().bg_noise_fading )
            {
                switch ( state_ )
                {
                    case nsFadeUp:
                        bg_noise_level_current_ = bg_noise_fade_rising_edge_[ bg_noise_edge_curr_++ ];
                        break;
                    case nsFadeDown:
                        bg_noise_level_current_ = bg_noise_fade_falling_edge_[ bg_noise_edge_curr_++ ];
                        break;
                    case nsUnchanging:
                        break;
                }

                if ( bg_noise_edge_curr_ >= cfg.d().bg_noise_fade_samples )
                {
                    bg_noise_edge_curr_ = 0;
                    state_ = nsUnchanging;
                }
            }

            // output_->write() returns true if the output buffer is full *after* the sample is added to the buffer
            bool output_buffer_full = output()->write( bg_noise_level_current_ * buffer_[ sample_curr_++ ] );

            if ( sample_curr_ >= samples_ )
            {
                sample_curr_ = 0;
            }

            if ( output_buffer_full )
            {
                break;
            }
        }
    }
}

bool
C_noise_file::initialise( shared_ptr< C_audio_output > output )
{
    bool worked = false;

    output_ = output;

    // Read in sample file
    if ( read() )
    {
        bg_noise_fade_rising_edge_  = new double[ cfg.d().bg_noise_fade_samples ];
        bg_noise_fade_falling_edge_ = new double[ cfg.d().bg_noise_fade_samples ];

        if ( bg_noise_fade_rising_edge_ && bg_noise_fade_rising_edge_ )
        {
            calculate_noise_envelopes();

            bg_noise_level_current_ = cfg.d().bg_noise ? ( cfg.d().bg_noise_fading ? cfg.d().bg_noise_level_quiescent
                                                                                   : cfg.d().bg_noise_level_active )
                                                       : 0.0;
            worked = true;
        }
        else
        {
            log_writeln( C_log::LL_ERROR, "Unable to allocate noise envelope buffers" );
        }
    }

    return worked;
}

bool
C_noise_file::read()
{
    // Only attempt to read the noise sound file if background noise is enabled
    return cfg.d().bg_noise ? C_sound_file::read() : true;
}

void
C_noise_file::calculate_noise_envelopes()
{
    // Calculate the envelopes for the background noise fade up and fade down
    double bg_noise_difference = cfg.d().bg_noise_level_active - cfg.d().bg_noise_level_quiescent;

    for ( unsigned int sample_count = 0; sample_count < cfg.d().bg_noise_fade_samples; sample_count++ )
    {
        // 'x axis'
        double x = ( ( double ) ( sample_count) / ( double ) cfg.d().bg_noise_fade_samples );       // 0 --> x --> 1

        // 'y axis'
        double y_fade_up = cfg.d().bg_noise_level_quiescent + ( x * bg_noise_difference );          // quiescent --> active

        bg_noise_fade_rising_edge_[ sample_count ]  = y_fade_up;

        // 'y axis'
        double y_fade_down = cfg.d().bg_noise_level_active - ( x * bg_noise_difference );           // active --> quiescent

        bg_noise_fade_falling_edge_[ sample_count ] = y_fade_down;
    }
}

}
