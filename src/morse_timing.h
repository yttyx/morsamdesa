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

// morse_timing.h

#ifndef morse_timing_H
#define morse_timing_H

#include "morse_enum.h"


namespace morsamdesa
{

class C_morse_timing
{
public:

    C_morse_timing( float char_wpm,        float effective_wpm
                  , float char_multiplier, float word_multiplier
                  , eMorseMode morse_mode, eMorseCode morse_code );

    ~C_morse_timing() {}

    unsigned int duration_dot() { return duration_dot_; }
    unsigned int duration_dash() { return duration_dash_; }
    unsigned int duration_dash_2() { return duration_dash_2_; }
    unsigned int duration_dash_3() { return duration_dash_3_; }
    unsigned int duration_element() { return duration_element_; }
    unsigned int duration_interelement() { return duration_interelement_; }
    unsigned int duration_interelement_2() { return duration_interelement_2_; }
    unsigned int duration_interletter() { return duration_interletter_; }
    unsigned int duration_interletter_fw() { return duration_interletter_fw_; }
    unsigned int duration_interword() { return duration_interword_; }
    unsigned int duration_interword_fw() { return duration_interword_fw_; }
    unsigned int duration_1min_check() { return duration_1min_check_; }

    unsigned int samples_dot() { return samples_dot_; }
    unsigned int samples_dash() { return samples_dash_; }
    unsigned int samples_dash_2() { return samples_dash_2_; }
    unsigned int samples_dash_3() { return samples_dash_3_; }
    unsigned int samples_interelement() { return samples_interelement_; }
    unsigned int samples_interelement_2() { return samples_interelement_2_; }
    unsigned int samples_interletter_fw() { return samples_interletter_fw_; }
    unsigned int samples_interletter() { return samples_interletter_; }
    unsigned int samples_interword_fw() { return samples_interword_fw_; }
    unsigned int samples_interword() { return samples_interword_; }

    void
    display();

    static void
    display_timing_examples();

private:

    unsigned int
    calculate_element_duration( float morse_speed_char );

    void
    calculate_farnsworth_delays( float        morse_speed_char
                               , float        morse_speed_effective
                               , unsigned int & intercharacter_delay
                               , unsigned int & interword_delay );

    unsigned int
    calculate_check_1min_duration( unsigned int duration_element
                                 , unsigned int duration_interletter
                                 , unsigned int duration_interword
                                 , float        effective_char_speed );

private:

    // Pulseaudio output durations
    unsigned int duration_dot_;                          // mS
    unsigned int duration_dash_;                         // mS
    unsigned int duration_dash_2_;                       // mS
    unsigned int duration_dash_3_;                       // mS
    unsigned int duration_element_;                      // mS
    unsigned int duration_interelement_;                 // mS
    unsigned int duration_interelement_2_;               // mS
    unsigned int duration_interletter_;                  // mS   actual (Farnsworth timing plus interval multiplication)
    unsigned int duration_interletter_fw_;               // mS   Farnsworth timing
    unsigned int duration_interword_;                    // mS   actual (Farnsworth timing plus interval multiplication)
    unsigned int duration_interword_fw_;                 // mS   Farnsworth timing
    unsigned int duration_1min_check_;                   // mS

    unsigned int samples_dot_;                           // Number of samples in a dot
    unsigned int samples_dash_;                          // Number of samples in a dash
    unsigned int samples_dash_2_;                        // Number of samples in a long dash         (American Morse only)
    unsigned int samples_dash_3_;                        // Number of samples in a even longer dash  (American Morse only)
    unsigned int samples_interelement_;                  // Number of samples between elements
    unsigned int samples_interelement_2_;                // Number of samples between elements       (American Morse only)
    unsigned int samples_interletter_fw_;                // Number of samples between letters
    unsigned int samples_interletter_;                   // Number of samples between letters
    unsigned int samples_interword_fw_;                  // Number of samples between words
    unsigned int samples_interword_;                     // Number of samples between words
};

}

#endif // morse_timing_H
