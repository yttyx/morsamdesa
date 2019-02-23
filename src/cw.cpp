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

// cw.cpp:

#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "cw.h"
#include "log.h"
#include "misc.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;


C_cw::C_cw( const S_transmitter & transmitter )
    : C_sample_source( transmitter.level )
    , transmitter_( transmitter )
{
    active_                  = false;
    buffer_                  = NULL;
    samples_                 = 0;
    sample_curr_             = 0;
    level_                   = 0.0;
    h1_level_                = 0.0;
    h2_level_                = 0.0;
    h3_level_                = 0.0;
    h4_level_                = 0.0;
    noise_level_             = 0.0;
    noise_burst_start_level_ = 0.0;
    noise_burst_end_level_   = 0.0;
    noise_burst_start_       = false;
    noise_burst_end_         = false;
    rising_edge_             = NULL;
    falling_edge_            = NULL;

    rising_edge_samples_  = ( SAMPLE_RATE * transmitter_.cw_rise_time ) / 1000;
    falling_edge_samples_ = ( SAMPLE_RATE * transmitter_.cw_fall_time ) / 1000;

    noise_burst_start_samples_ = ( SAMPLE_RATE * transmitter_.cw_noise_burst_start_duration ) / 1000;
    noise_burst_end_samples_   = ( SAMPLE_RATE * transmitter_.cw_noise_burst_end_duration ) / 1000;

    noise_burst_start_level_ = transmitter_.cw_noise_burst_start_level;
    noise_burst_end_level_   = transmitter_.cw_noise_burst_end_level;
    noise_burst_start_       = noise_burst_start_level_ > 0.0;
    noise_burst_end_         = noise_burst_end_level_ > 0.0;

    level_    = transmitter_.level;
    h1_level_ = transmitter_.cw_h1_level;
    h2_level_ = transmitter_.cw_h2_level;
    h3_level_ = transmitter_.cw_h3_level;
    h4_level_ = transmitter_.cw_h4_level;
}

C_cw::~C_cw()
{
    release_edge_envelopes();
}

/* /brief Add some sound samples to an output buffer
 *
 * /detail Supply of samples is based around supplying bursts of tick_samples samples or fewer
 *
 */
void
C_cw::write( bool & samples_exhausted )
{
    samples_exhausted = false;

    if ( active_ )
    {
        for ( ;; )
        {
            // output_->write() returns true if the output buffer is full *after* the sample is added to the buffer
            bool output_buffer_full = output()->write( level_ * buffer_[ sample_curr_++ ] );

            if ( sample_curr_ >= samples_ )
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

/* /brief Set up buffers, envelope arrays and background noise
 *
 */
bool
C_cw::initialise( unsigned int samples, shared_ptr< C_audio_output > output, int frequency_shift )
{
    output_ = output;

    bool worked = init_edge_envelopes() && init_buffer( samples, frequency_shift );
    
    if ( worked )
    {
        release_edge_envelopes();       // No longer required
    }
    else
    {
        log_writeln( C_log::LL_ERROR, "**C_cw::initialise(): failed" );
        free_resources();
    }

    return worked;
}

// Reference for envelope calculation:
//   https://www.dr-lex.be/info-stuff/volumecontrols.html
//
//

/* /brief Calculate the arrays of level multipliers which are used for CW envelope shaping
 *
 */
bool
C_cw::init_edge_envelopes()
{
    rising_edge_  = new double[ rising_edge_samples_ ];
    falling_edge_ = new double[ falling_edge_samples_ ];

    if ( ( ! rising_edge_ ) || ( ! falling_edge_ ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Error allocating edge envelope buffer: %s", strerror( errno ) );
        return false;
    }

    // For each sample in a CW rising edge, calculate the multiplier which will be applied later to the
    // CW mnemonic sample amplitude to give a rising edge envelope.
    for ( unsigned int sample_count = 0; sample_count < rising_edge_samples_; sample_count++ )
    {
        double x = ( ( double ) sample_count ) / ( ( double ) rising_edge_samples_ );           // 0 --> x --> 1
        
        rising_edge_[ sample_count ] = pow( x, 3 );                                             // 0 ......... 1
    }

    // For each sample in a CW falling edge, calculate the multiplier which will be applied later to the
    // CW mnemonic sample amplitude to give a falling edge envelope.
    for ( unsigned int sample_count = 0; sample_count < falling_edge_samples_; sample_count++ )
    {
        double x = ( ( double ) sample_count ) / ( ( double ) falling_edge_samples_ );          // 0 --> x --> 1

        falling_edge_[ sample_count ] = pow( 1.0 - x, 3 );                                      // 1 ......... 0
    }

    return true;
}

void
C_cw::release_edge_envelopes()
{
    delete [] rising_edge_;
    rising_edge_ = NULL;

    delete [] falling_edge_;
    falling_edge_ = NULL;
}

bool
C_cw::init_buffer( unsigned int samples, int frequency_shift )
{
    return generate_sample_buffer( samples, frequency_shift ) && shape_waveform();
};

/* /brief Populate the signal buffer using the supplied parameters
 *
 * /param type              Dit or dash
 * /param frequency_shift   Number of semitones relative to the fundamental frequency
  */
bool
C_cw::generate_sample_buffer( unsigned int samples, int frequency_shift )
{
    // Calculate the required buffer size
    samples_ = samples;

    log_writeln_fmt( C_log::LL_VERBOSE_1, "frequency_shift     : %d", frequency_shift );
    log_writeln_fmt( C_log::LL_VERBOSE_1, "Allocating CW buffer: samples: %u", samples_ );

    buffer_ = new short int[ samples_ ];

    if ( buffer_ )
    {
        double frequency = transmitter_.cw_h1_frequency;

        frequency = frequency * ( pow( 1.059463, frequency_shift ) );

        for ( unsigned int sample = 0; sample < samples_; sample++ )
        {
            double sample_level = calc_sample_level( sample, frequency );

            buffer_[ sample ] = sample_level * TONE_AMPLITUDE;
        }

        return true;
    }
    else
    {
        log_writeln_fmt( C_log::LL_ERROR, "C_cw::generate_sample_buffer(): Error allocating buffer: %s", strerror( errno ) );
    }

    return false;
}

/* /brief Calculate one CW sample level
 *
 * /detail Calculate a level (multiplier) for one sample of a sinusoidal CW waveform, with addotional sinusoidal harmonics added if configured
 */
double
C_cw::calc_sample_level( unsigned int sample_count, double frequency )
{
    double sample_level = ( ( h1_level_ * sin( 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) )
                          + ( h2_level_ * sin( transmitter_.cw_h2_multiplier * 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) )
                          + ( h3_level_ * sin( transmitter_.cw_h3_multiplier * 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) )
                          + ( h4_level_ * sin( transmitter_.cw_h4_multiplier * 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) ) );
    return sample_level;
}

bool
C_cw::shape_waveform()
{
    for ( unsigned int sample = 0; sample < samples_; sample++ )
    {
        double    sample_mult  = 1.0;
        short int sample_value = 0;

        sample_value = buffer_[ sample ];

        if ( sample < rising_edge_samples_ )
        {
            // Add a short burst of noise to the start of a CW element if configured.
            // NB: This is not affected by the leading edge envelope.
            if ( noise_burst_start_  && ( sample < noise_burst_start_samples_ ) )
            {
                sample_value += ( noise_burst_start_level_ * CW_NOISE_AMPLITUDE * ( rand() / (1.0 * RAND_MAX ) - 0.5 ) );
            }
            else
            {
                // Shape rising edge
                sample_mult = rising_edge_[ sample ];
            }
        }
        else if ( sample < ( samples_ - falling_edge_samples_ ) )
        {
            // CW maximum level, no shaping required
        }
        else
        {
            if ( noise_burst_end_ && ( sample < ( samples_ - noise_burst_end_samples_ ) ) )
            {
                sample_value += ( noise_burst_end_level_ * CW_NOISE_AMPLITUDE * ( rand() / (1.0 * RAND_MAX ) - 0.5 ) );
            }
            else
            {
                // Shape falling edge
                sample_mult = falling_edge_[ sample - ( samples_ - falling_edge_samples_ ) ];
            }
        }

        buffer_[ sample ] = sample_mult * sample_value;
    }

    return true;
}

void
C_cw::free_resources()
{
    free_buffer( &rising_edge_ );
    free_buffer( &falling_edge_ );
}

void
C_cw::free_buffer( short int **buffer )
{
    delete [] *buffer;
    buffer = NULL;
}

void
C_cw::free_buffer( double **buffer )
{
    delete [] *buffer;
    buffer = NULL;
}

}
