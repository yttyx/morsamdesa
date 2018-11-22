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

// tone.cpp:

#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "tone.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;

// Used for CW and command acknowledgement tones
static S_tone_def tone_dit        = { ".",    { eNoChange, eInvalid                                } };
static S_tone_def tone_dah        = { "-",    { eNoChange, eInvalid                                } };
static S_tone_def tone_unmute     = { "..-",  { eUp5th, eNoChange, eNoChange, eInvalid             } };  // Unmute
static S_tone_def tone_mute       = { "--",   { eUp5th, eNoChange, eInvalid                        } };  // Mute
static S_tone_def tone_interrupt  = { "...",  { eUp5th, eNoChange, eNoChange,  eInvalid            } };  // Stop current message
static S_tone_def tone_previous   = { ".--.", { eUp5th, eNoChange, eNoChange, eNoChange, eInvalid  } };  // play Previous message
static S_tone_def tone_next       = { "-.",   { eUp5th, eNoChange, eInvalid                        } };  // play Next message

// Used for sounder (command acknowledgement tones only)
static S_tone_def one_beep        = { ".",    { eNoChange, eInvalid                                } };  // Mute / Next / Prev / Int
static S_tone_def two_beeps       = { "..",   { eNoChange, eNoChange, eInvalid                     } };  // Unmute


C_tone::C_tone()
{
    active_                     = false;
    buffer_                     = NULL;
    samples_                    = 0;
    sample_curr_                = 0;
    cw_h1_level_                = 0;
    cw_h2_level_                = 0;
    cw_h3_level_                = 0;
    cw_h4_level_                = 0;
    cw_noise_level_             = 0;
    cw_noise_burst_start_level_ = 0;
    cw_noise_burst_end_level_   = 0;
    cw_noise_burst_start_       = false;
    cw_noise_burst_end_         = true;
    cw_rising_edge_             = NULL;
    cw_falling_edge_            = NULL;
}

C_tone::~C_tone()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_tone destructor" );

    release_edge_envelopes();
}

/* /brief Add some sound samples to an output buffer
 *
 * /detail Supply of samples is based around supplying bursts of tick_samples samples or fewer
 *
 */
void
C_tone::write( bool & samples_exhausted )
{
    samples_exhausted = false;

    if ( active_ )
    {
        for ( ;; )
        {
            // output_->write() returns true if the output buffer is full *after* the sample is added to the buffer

            bool output_buffer_full = output()->write( buffer_[ sample_curr_++ ] );

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
C_tone::initialise( eToneType tone_type, C_audio_output * output )
{
    output_    = output;

    bool worked = init_edge_envelopes() && init_tone_buffer( tone_type );
    
    if ( worked )
    {
        release_edge_envelopes();       // No longer required
    }
    else
    {
        log_writeln( C_log::LL_ERROR, "**C_tone::initialise(): failed" );
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
C_tone::init_edge_envelopes()
{
    cw_rising_edge_  = new double[ cfg.d().cw_rising_edge_samples ];
    cw_falling_edge_ = new double[ cfg.d().cw_falling_edge_samples ];

    if ( ( ! cw_rising_edge_ ) || ( ! cw_falling_edge_ ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Error allocating edge envelope buffer: %s", strerror( errno ) );
        return false;
    }

    // For each sample in a CW rising edge, calculate the multiplier which will be applied later to the
    // CW source sample amplitude to give a rising edge envelope.

    for ( unsigned int sample_count = 0; sample_count < cfg.d().cw_rising_edge_samples; sample_count++ )
    {
        double x = ( ( double ) sample_count ) / ( ( double ) cfg.d().cw_rising_edge_samples );    // 0 --> x --> 1
        
        cw_rising_edge_[ sample_count ] = pow( x, 3 );                                             // 0 ......... 1
    }

    // For each sample in a CW falling edge, calculate the multiplier which will be applied later to the
    // CW source sample amplitude to give a falling edge envelope.
    for ( unsigned int sample_count = 0; sample_count < cfg.d().cw_falling_edge_samples; sample_count++ )
    {
        double x = ( ( double ) sample_count ) / ( ( double ) cfg.d().cw_falling_edge_samples );   // 0 --> x --> 1

        cw_falling_edge_[ sample_count ] = pow( 1.0 - x, 3 );                                      // 1 ......... 0
    }

    return true;
}

void
C_tone::release_edge_envelopes()
{
    if ( cw_rising_edge_ )
    {
        delete [] cw_rising_edge_;
        cw_rising_edge_ = NULL;
    }
    if ( cw_falling_edge_ )
    {
        delete [] cw_falling_edge_;
        cw_falling_edge_ = NULL;
    }
}

bool
C_tone::init_tone_buffer( eToneType tone_type )
{
    // Convert the linear level settings to exponential values between 0 and 1
    cw_h1_level_                = cfg.d().cw_h1_level;
    cw_h2_level_                = cfg.d().cw_h2_level;
    cw_h3_level_                = cfg.d().cw_h3_level;
    cw_h4_level_                = cfg.d().cw_h4_level;

    cw_noise_level_             = cfg.d().cw_noise_level;
    cw_noise_burst_start_level_ = cfg.d().cw_noise_burst_start_level;
    cw_noise_burst_end_level_   = cfg.d().cw_noise_burst_end_level;

    cw_noise_burst_start_       = cw_noise_burst_start_level_ > 0.0;
    cw_noise_burst_end_         = cw_noise_burst_end_level_ > 0.0;

    S_tone_def *tone_def = NULL;
    
    bool padding   = false;
    bool harmonics = false;

    get_tone_settings( tone_type, tone_def, padding, harmonics );

    bool worked = generate_sample_buffer( *tone_def, harmonics, padding );

    if ( worked && ( ( tone_type == ttDit ) || ( tone_type == ttDah ) ) )
    {
        complete_cw_tone();
    }

    return worked;
};

void
C_tone::get_tone_settings( eToneType tone_type, S_tone_def *& tone_def, bool & padding, bool & harmonics )
{
    padding   = true;
    harmonics = false;

    if ( cfg.d().morse_mode == MM_CW )
    {
        switch ( tone_type )
        {
            case ttDit:
                tone_def = &tone_dit;
                padding = false;
                harmonics = true;
                break;
            case ttDah:
                tone_def = &tone_dah;
                padding = false;
                harmonics = true;
                break;
            case ttUnmute:
                tone_def = &tone_unmute;
                break;
            case ttMute:
                tone_def = &tone_mute;
                break;
            case ttInterrupt:
                tone_def = &tone_interrupt;
                break;
            case ttPrevious:
                tone_def = &tone_previous;
                break;
            case ttNext:
                tone_def = &tone_next;
                break;
        }
    }
    else
    {
        switch ( tone_type )
        {
            case ttDit:
                tone_def = &tone_dit;
                padding = false;
                harmonics = true;
                break;
            case ttDah:
                tone_def = &tone_dah;
                padding = false;
                harmonics = true;
                break;
            case ttUnmute:
                tone_def = &two_beeps;
                break;
            case ttMute:
                tone_def = &one_beep;
                break;
            case ttInterrupt:
                tone_def = &one_beep;
                break;
            case ttPrevious:
                tone_def = &one_beep;
                break;
            case ttNext:
                tone_def = &one_beep;
                break;
        }
    }
}

/* /brief Populate the signal buffer using the supplied parameters
 *
 * /param tones      An array of intervals, relative to the pitch of the CW (first entry), then
 *                   relative to the pitch of the preceding entry (second and subsequent entries)
 * /param harmonics  If true, add harmonics to the signal fundamental (if configured)
 * /param padding    If true, add some silence to the start and end of the generated samples
 */
bool
C_tone::generate_sample_buffer( S_tone_def & tones, bool harmonics, bool padding )
{
    // Calculate the required buffer size
    unsigned int sample_count = 0;

    for ( unsigned int ii = 0; ii < strlen( tones.morse ); ii++ )
    {
        sample_count += ( tones.morse[ ii ] == '.' ? cfg.d().dit_samples : cfg.d().dah_samples );
        
        if ( tones.morse[ ii + 1 ] )
        {
            sample_count += cfg.d().interelement_samples;
        }
    }

    if ( padding )
    {
        sample_count += ( 2 * cfg.d().dah_samples );
    }

    log_writeln_fmt( C_log::LL_VERBOSE_3, "Allocating tone buffer: Morse: %s, samples: %u", tones.morse, sample_count );

    buffer_  = new short int[ sample_count ];
    samples_ = sample_count;

    if ( buffer_ )
    {
        unsigned int buffer_pos = 0;

        if ( padding )
        {
            // Short leading silence
            memset( &buffer_[ buffer_pos ], 0, cfg.d().dah_samples * sizeof( short int ) );
            buffer_pos += cfg.d().dah_samples;
        }

        double frequency = cfg.c().morse_cw_h1_frequency;

        for ( unsigned int element = 0; element < strlen( tones.morse ); element++ )
        {
            // Calculate next frequency, relative to the current frequency
            double frequency_temp = frequency;

            frequency = frequency * ( pow( 1.059463, ( int ) tones.intervals[ element ] ) );

            log_writeln_fmt( C_log::LL_VERBOSE_3, "  frequency (before): %f    (after): %f", frequency_temp, frequency );

            // Generate tone
            unsigned int element_samples = tones.morse[ element ] == '.' ? cfg.d().dit_samples : cfg.d().dah_samples;

            for ( unsigned int sample = 0; sample < element_samples; sample++ )
            {
                double sample_level = calc_sample_level( sample, frequency, harmonics );

                buffer_[ buffer_pos++ ] = sample_level * TONE_AMPLITUDE;
            }

            if ( tones.morse[ element + 1 ] )
            {
                // Inter-element silence
                memset( &buffer_[ buffer_pos ], 0, cfg.d().interelement_samples * sizeof( short int ) );
                buffer_pos += cfg.d().interelement_samples;
            }
        }

        if ( padding )
        {
            // Short trailing silence
            memset( &buffer_[ buffer_pos ], 0, cfg.d().dah_samples * sizeof( short int ) );
            buffer_pos += cfg.d().dah_samples;
        }

        return true;
    }
    else
    {
        log_writeln_fmt( C_log::LL_ERROR, "Error allocating buffer: %s", strerror( errno ) );
    }

    return false;
}

/* /brief Calculate one CW sample level
 *
 * /detail Calculate a level (multiplier) for one sample of a sinusoidal CW waveform, with additional sinusoidal harmonics added if configured
 */
double
C_tone::calc_sample_level( unsigned int sample_count, double frequency, bool harmonics )
{
    double sample_level = harmonics ? ( ( cw_h1_level_   * sin( 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) )
                                        + ( cw_h2_level_ * sin( cfg.c().morse_cw_h2_multiplier * 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) )
                                        + ( cw_h3_level_ * sin( cfg.c().morse_cw_h3_multiplier * 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) )
                                        + ( cw_h4_level_ * sin( cfg.c().morse_cw_h4_multiplier * 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) ) )
                                    :   ( cw_h1_level_ * sin( 2 * M_PI * frequency * sample_count / SAMPLE_RATE ) );

    return sample_level;
}

/* /brief Add leading/trailing edge shaping and noise to CW tone if configured
 *
 */
void
C_tone::complete_cw_tone()
{
    for ( unsigned int sample = 0; sample < samples_; sample++ )
    {
        double    sample_mult  = 1.0;
        short int sample_value = 0;

        sample_value = buffer_[ sample ];

        if ( sample < cfg.d().cw_rising_edge_samples )
        {
            // Add a short burst of noise to the start of a CW element if configured.
            // NB: This is not affected by the leading edge envelope.
            if ( cw_noise_burst_start_  && ( sample < cfg.d().cw_burst_noise_start_samples ) )
            {
                sample_value += ( cw_noise_burst_start_level_ * CW_NOISE_AMPLITUDE * ( rand() / (1.0 * RAND_MAX ) - 0.5 ) );
            }
            else
            {
                // Shape rising edge
                sample_mult = cw_rising_edge_[ sample ];
            }
        }
        else if ( sample < ( samples_ - cfg.d().cw_falling_edge_samples ) )
        {
            // CW maximum level, no shaping required
        }
        else
        {
            if ( cw_noise_burst_end_ && ( sample < ( samples_ - cfg.d().cw_burst_noise_end_samples ) ) )
            {
                sample_value += ( cw_noise_burst_end_level_ * CW_NOISE_AMPLITUDE * ( rand() / (1.0 * RAND_MAX ) - 0.5 ) );
            }
            else
            {
                // Shape falling edge
                sample_mult = cw_falling_edge_[ sample - ( samples_ - cfg.d().cw_falling_edge_samples ) ];
            }
        }

        buffer_[ sample ] = sample_mult * sample_value;
    }
}

void
C_tone::free_resources()
{
    free_buffer( &cw_rising_edge_ );
    free_buffer( &cw_falling_edge_ );
}

void
C_tone::free_buffer( short int **buffer )
{
    if ( *buffer )
    {
        delete [] *buffer;
        buffer = NULL;
    }
}

void
C_tone::free_buffer( double **buffer )
{
    if ( *buffer )
    {
        delete [] *buffer;
        buffer = NULL;
    }
}

/* /brief
 *
 */
void
C_tone::dump_buffer( const char * desc, short int * buffer, unsigned int samples )
{
    log_writeln( C_log::LL_INFO, "----------------------------------------------------------------------" );
    log_writeln_fmt( C_log::LL_INFO, "Buffer: %s, buffer: %p, length %u", desc, buffer, samples );

    unsigned int col = 0;

    for ( unsigned int ii = 0; ii < samples; ii++ )
    {
        log_write_fmt( C_log::LL_INFO, "  %5d", buffer[ ii ] );

        if ( ( col++ % 8 ) == 0 )
        {
            log_writeln( C_log::LL_INFO, "" );
        }
    }
    log_writeln( C_log::LL_INFO, "" );
    log_writeln( C_log::LL_INFO, "----------------------------------------------------------------------" );
}

}
