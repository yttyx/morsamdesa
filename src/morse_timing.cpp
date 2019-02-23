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

// morse_timing.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "morse_timing.h"
#include "log.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;


C_morse_timing::C_morse_timing( float char_wpm,        float effective_wpm
                              , float char_multiplier, float word_multiplier
                              , eMorseMode morse_mode, eMorseCode morse_code )
{
    // With reference to 'A Standard for Morse Timing Using the  Farnsworth Technique.pdf'
    //   http://www.arrl.org/files/file/Technology/x9004008.pdf
    //
    // Using the 'PARIS' 50 unit standard
    //
    //            u = 1.2 / c
    //
    // where u: period of one unit in seconds
    //       c: speed of transmission, in words per minute
    //
    // '+ 5' and '+ 50' - round up to nearest millisecond. All settings are in milliseconds.
    //
    // duration_dot_          = ( ( ( 1000L * 12 ) / wpm ) + 5 ) / 10;
    // duration_dash_         = duration_dot_ * 3;
    // duration_interelement_ = duration_dot_;
    // duration_interletter_  = ( ( ( duration_dot_ * 3 ) * get_multiplier_x100( letter_delay ) ) + 50 ) / 100
    // duration_interword_    = ( ( ( duration_dot_ * 7 ) * get_multiplier_x100( word_delay   ) ) + 50 ) / 100
    //
    // Calculate Farnsworth delay
    //
    // ta is the total delay to add to the characters (31 units) of a standard 50 unit word (PARIS), in seconds
    // ta = ( 60c - 37.2s ) / ( sc )
    // ta = ( ( 60 * char_speed ) - ( 37.2 * effective_speed ) ) / ( char_speed * effective_speed )

    // TODO Farnsworth calculation for American Morse code

    // ------------------------------------------------------------------------------------------------------------
    // Audio duration calculations
    // ------------------------------------------------------------------------------------------------------------

    duration_element_ = calculate_element_duration( char_wpm );

    calculate_farnsworth_delays( char_wpm, effective_wpm, duration_interletter_fw_, duration_interword_fw_ );

    duration_dot_ = duration_element_;

    unsigned int dash_multiplier = 3;

    if ( morse_mode == MM_DOUBLE_PLATE_SOUNDER )
    {
        dash_multiplier = 1;
    }
    else if ( morse_code == MC_AMERICAN )
    {
        dash_multiplier = 2;
    }

    duration_dash_           = duration_element_ * dash_multiplier;
    duration_dash_2_         = duration_dash_ * 2;
    duration_dash_3_         = duration_dash_ * 3;
    duration_interelement_   = duration_element_;
    duration_interelement_2_ = duration_element_ * 2;

    // Multiply back up the calculated values to check if they seem plausible
    //
    // Standard PARIS word consists of:
    //   31 elements including inter-element spacing
    //    4 inter-character delays
    //    1 inter-word delay
    //
    // So we should get:
    // ( ( 31 * duration_element_ ) + ( 4 * duration_interletter_ ) + duration_interword_ ) * effective_speed == 60000 (milliseconds)

    duration_1min_check_ = calculate_check_1min_duration( duration_element_,
                                                          duration_interletter_fw_,
                                                          duration_interword_fw_,
                                                          effective_wpm );

    // Apply any further character and word multipliers
    duration_interletter_ = char_multiplier * duration_interletter_fw_;
    duration_interword_   = word_multiplier * duration_interword_fw_;

    // Convert durations to sample counts
    samples_dot_            = ( SAMPLE_RATE * duration_dot_ ) / 1000;
    samples_dash_           = ( SAMPLE_RATE * duration_dash_ ) / 1000;
    samples_dash_2_         = ( ( SAMPLE_RATE * duration_dash_ ) / 1000 ) * 2;
    samples_dash_3_         = ( ( SAMPLE_RATE * duration_dash_ ) / 1000 ) * 3;
    samples_interelement_   = ( SAMPLE_RATE * duration_interelement_ ) / 1000;
    samples_interelement_2_ = ( ( SAMPLE_RATE * duration_interelement_ ) / 1000 ) * 2;
    samples_interletter_fw_ = ( SAMPLE_RATE * duration_interletter_fw_ ) / 1000;
    samples_interletter_    = ( SAMPLE_RATE * duration_interletter_ ) / 1000;
    samples_interword_fw_   = ( SAMPLE_RATE * duration_interword_fw_ ) / 1000;
    samples_interword_      = ( SAMPLE_RATE * duration_interword_ ) / 1000;
}

unsigned int
C_morse_timing::calculate_element_duration( float morse_speed_char )
{
    return ( unsigned int ) ( ( ( float ) ( 1000L * 12 ) / morse_speed_char ) + 5 ) / 10;
}

void
C_morse_timing::calculate_farnsworth_delays( float        char_wpm
                                           , float        effective_wpm
                                           , unsigned int & intercharacter_delay
                                           , unsigned int & interword_delay )
{
    // Calculate Farnsworth delays
    //
    // total_delay is the overall delay to add to the characters (31 units) of a standard 50 unit word (PARIS), in seconds
    // total_delay = ( 60c - 37.2s ) / ( sc )
    // total_delay = ( ( 60 * character speed ) - ( 37.2 * effective speed ) ) / ( character speed * effective_speed )
    //
    // then:
    // intercharacter_delay = ( 3 * total_delay ) / 19
    // and:
    // interword_delay      = ( 7 * total_delay ) / 19

    // * 1000 for milliseconds
    unsigned int total_delay = ( unsigned int ) ( ( ( ( 60.0 * char_wpm ) - ( ( 372.0 * effective_wpm ) / 10.0 ) ) * 1000.0 ) / ( char_wpm * effective_wpm ) );   // mS

    intercharacter_delay = ( 3 * total_delay ) / 19;       // mS
    interword_delay      = ( 7 * total_delay ) / 19;       // mS
}

unsigned int
C_morse_timing::calculate_check_1min_duration( unsigned int duration_element
                                             , unsigned int duration_interletter
                                             , unsigned int duration_interword
                                             , float        effective_char_speed )
{
    return ( unsigned int )( ( float ) ( ( ( 31 * duration_element ) + ( 4 * duration_interletter ) + duration_interword ) ) * effective_char_speed );
}


void
C_morse_timing::display()
{
    //TODO
    //log_writeln( C_log::LL_INFO, "Audio" );
    //log_writeln_fmt( C_log::LL_INFO, "  Dot duration                      : %5u mS",                     derived_.audio_duration_dot               );
    //log_writeln_fmt( C_log::LL_INFO, "  Dash duration                     : %5u mS",                     derived_.audio_duration_dash              );
    //log_writeln_fmt( C_log::LL_INFO, "  Interelement duration             : %5u mS",                     derived_.audio_duration_interelement      );
    //log_writeln_fmt( C_log::LL_INFO, "  Interletter duration (Farnsworth) : %5u mS",                     derived_.audio_duration_interletter_fw    );
    //log_writeln_fmt( C_log::LL_INFO, "  Interletter duration (actual)     : %5u mS",                     derived_.audio_duration_interletter       );
    //log_writeln_fmt( C_log::LL_INFO, "  Interword duration (Farnsworth)   : %5u mS",                     derived_.audio_duration_interword_fw      );
    //log_writeln_fmt( C_log::LL_INFO, "  Interword duration (actual)       : %5u mS",                     derived_.audio_duration_interword         );
    //log_writeln_fmt( C_log::LL_INFO, "  Check value                       : %5u mS (should be ~60000 )", derived_.audio_duration_1min_check        );

    //log_writeln( C_log::LL_INFO, "LED" );
    //log_writeln_fmt( C_log::LL_INFO, "  Dot duration                      : %5u mS",                     derived_.led_duration_dot                 );
    //log_writeln_fmt( C_log::LL_INFO, "  Dash duration                     : %5u mS",                     derived_.led_duration_dash                );
    //log_writeln_fmt( C_log::LL_INFO, "  Interelement duration             : %5u mS",                     derived_.led_duration_interelement        );
    //log_writeln_fmt( C_log::LL_INFO, "  Interletter duration (Farnsworth) : %5u mS",                     derived_.led_duration_interletter_fw      );
    //log_writeln_fmt( C_log::LL_INFO, "  Interletter duration (actual)     : %5u mS",                     derived_.led_duration_interletter         );
    //log_writeln_fmt( C_log::LL_INFO, "  Interword duration (Farnsworth)   : %5u mS",                     derived_.led_duration_interword_fw        );
    //log_writeln_fmt( C_log::LL_INFO, "  Interword duration (actual)       : %5u mS",                     derived_.led_duration_interword           );
    //log_writeln_fmt( C_log::LL_INFO, "  Check value                       : %5u mS (should be ~60000 )", derived_.led_duration_1min_check          );
    //log_writeln_fmt( C_log::LL_INFO, "  Dit samples                       : %6u",                        derived_.dot_samples                      );
    //log_writeln_fmt( C_log::LL_INFO, "  Dah samples                       : %6u",                        derived_.dash_samples                      );
    //log_writeln_fmt( C_log::LL_INFO, "  Interelement samples              : %6u",                        derived_.interelement_samples             );
    //log_writeln_fmt( C_log::LL_INFO, "  Interletter samples (Farnsworth)  : %6u",                        derived_.interletter_samples_fw           );
    //log_writeln_fmt( C_log::LL_INFO, "  Interletter samples               : %6u",                        derived_.interletter_samples              );
    //log_writeln_fmt( C_log::LL_INFO, "  Interword samples (Farnsworth)    : %6u",                        derived_.interword_samples_fw             );
    //log_writeln_fmt( C_log::LL_INFO, "  Interword samples                 : %6u",                        derived_.interword_samples                );
    //log_writeln_fmt( C_log::LL_INFO, "  CW rising edge samples            : %6u",                        derived_.cw_rising_edge_samples           );
    //log_writeln_fmt( C_log::LL_INFO, "  CW falling edge samples           : %6u",                        derived_.cw_falling_edge_samples          );
    //log_writeln_fmt( C_log::LL_INFO, "  CW burst start samples            : %6u",                        derived_.cw_burst_noise_start_samples     );
    //log_writeln_fmt( C_log::LL_INFO, "  CW burst end samples              : %6u",                        derived_.cw_burst_noise_end_samples       );
    //log_writeln_fmt( C_log::LL_INFO, "  CW harmonic 1 level               : %1.3f",                      derived_.cw_h1_level                      );
    //log_writeln_fmt( C_log::LL_INFO, "  CW harmonic 2 level               : %1.3f",                      derived_.cw_h2_level                      );
    //log_writeln_fmt( C_log::LL_INFO, "  CW harmonic 3 level               : %1.3f",                      derived_.cw_h3_level                      );
    //log_writeln_fmt( C_log::LL_INFO, "  CW harmonic 4 level               : %1.3f",                      derived_.cw_h4_level                      );
    //log_writeln_fmt( C_log::LL_INFO, "  CW noise burst start level        : %1.3f",                      derived_.cw_noise_burst_start_level       );
    //log_writeln_fmt( C_log::LL_INFO, "  CW noise burst end level          : %1.3f",                      derived_.cw_noise_burst_end_level         );
}

void
C_morse_timing::display_timing_examples()
{
    log_writeln( C_log::LL_INFO, "WPM  dot    dash   check" );
    log_writeln( C_log::LL_INFO, "---  -----  -----  -----" );

    for ( float wpm = 5.0; wpm < 26.0; wpm += 1.0 )
    {
        C_morse_timing mt( wpm, wpm, 1.0, 1.0, MM_CW, MC_INTERNATIONAL );

        log_writeln_fmt( C_log::LL_INFO, "%3.0f  %5u  %5u  %5u"
                                        , wpm
                                        , mt.duration_dot()
                                        , mt.duration_dash()
                                        , mt.duration_1min_check() );
    }                               
}

}
