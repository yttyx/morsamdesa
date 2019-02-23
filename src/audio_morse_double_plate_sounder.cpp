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

// audio_morse_double_plate_sounder.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "audio_morse_double_plate_sounder.h"
#include "log.h"
#include "misc.h"


using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config  cfg;
extern C_log     log;


C_audio_morse_double_plate_sounder::C_audio_morse_double_plate_sounder( const S_transmitter & transmitter )
    : C_audio_morse( transmitter )
{
    sending_      = false;
    interrupt_    = false;
    element_curr_ = meNone;
}

C_audio_morse_double_plate_sounder::~C_audio_morse_double_plate_sounder()
{
}

bool
C_audio_morse_double_plate_sounder::initialise( shared_ptr< C_audio_output > output )
{
    C_audio_morse::initialise();

    // For the simulation of the double plate sounder, the hi and lo sounder samples each contain both the armature up and armature
    // down sounds, and the sample is allowed to run on into the following inter<element,letter,word> silence. As a result there will
    // be an optimal transmission speed for a given set of hi and lo sounder samples.

    dot_interelement_.reset( new C_sounder( "dot_interelement_", transmitter_.sounder_hi, morse_timing_->samples_dot() + morse_timing_->samples_interelement(), transmitter_.level ) );
    dot_interletter_.reset( new C_sounder( "dot_interletter_", transmitter_.sounder_hi, morse_timing_->samples_dot() + morse_timing_->samples_interletter(), transmitter_.level ) );
    dot_interword_.reset( new C_sounder( "dot_interword_", transmitter_.sounder_hi, morse_timing_->samples_dot() + morse_timing_->samples_interword(), transmitter_.level ) );

    dash_interelement_.reset( new C_sounder( "dash_interelement_", transmitter_.sounder_lo, morse_timing_->samples_dash() + morse_timing_->samples_interelement(), transmitter_.level ) );
    dash_interletter_.reset( new C_sounder( "dash_interletter_", transmitter_.sounder_lo, morse_timing_->samples_dash() + morse_timing_->samples_interletter(), transmitter_.level ) );
    dash_interword_ .reset( new C_sounder( "dash_interword_", transmitter_.sounder_lo, morse_timing_->samples_dash() + morse_timing_->samples_interword(), transmitter_.level ) );

    bool worked = true;

    worked = worked && dot_interelement_->initialise( output );
    worked = worked && dot_interletter_->initialise( output );
    worked = worked && dot_interword_->initialise( output );

    worked = worked && dash_interelement_->initialise( output );
    worked = worked && dash_interletter_->initialise( output );
    worked = worked && dash_interword_->initialise( output );

    if ( ! worked )
    {
        log_writeln( C_log::LL_ERROR, "C_audio_morse_double_plate_sounder initialisation error" );
    }

    return worked;
}

void
C_audio_morse_double_plate_sounder::select_element_sound( eMorseElement element )
{
    log_writeln_fmt( C_log::LL_VERBOSE_1, "element: %s", to_string( element ) );
    
    eMorseElement element_next = meNone;

    switch ( element )
    {
        case meDit:
        case meDah:
            element_next = text_to_morse_->get_element();
            break;
        default:
            break;
    }

    if ( element_next != meNone )
    {
        switch ( element_next )
        {
            case meInterElement:
                sample_source_ = ( element == meDit ) ? dot_interelement_ : dash_interelement_;
                break;
            case meInterCharacter:
            case meEndOfMessage:
                sample_source_ = ( element == meDit ) ? dot_interletter_ : dash_interletter_;
                break;
            case meInterWord:
                sample_source_ = ( element == meDit ) ? dot_interword_ : dash_interword_;
                break;
            default:
                sample_source_ = dummy_;
                break;
        }

        if ( sample_source_ )
        {
            sample_source_->trigger();
        }
    }
}

}
