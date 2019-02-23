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

// audio_morse_sparkgap.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "audio_morse_sparkgap.h"
#include "log.h"
#include "sparkgap.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config  cfg;
extern C_log     log;


C_audio_morse_sparkgap::C_audio_morse_sparkgap( const S_transmitter & transmitter )
    : C_audio_morse( transmitter )
{
}

C_audio_morse_sparkgap::~C_audio_morse_sparkgap()
{
}

bool
C_audio_morse_sparkgap::initialise( shared_ptr< C_audio_output > output )
{
    C_audio_morse::initialise();

    dot_interelement_.reset( new C_sparkgap( "dot_interelement_", transmitter_.sparkgap_dot, morse_timing_->samples_dot() + morse_timing_->samples_interelement(), transmitter_.level ) );
    dot_interelement_2_.reset( new C_sparkgap( "dot_interelement_2_", transmitter_.sparkgap_dot, morse_timing_->samples_dot() + morse_timing_->samples_interelement_2(), transmitter_.level ) );
    dot_interletter_.reset( new C_sparkgap( "dot_interletter_", transmitter_.sparkgap_dot, morse_timing_->samples_dot() + morse_timing_->samples_interletter(), transmitter_.level ) );
    dot_interword_.reset( new C_sparkgap( "dot_interword_", transmitter_.sparkgap_dot, morse_timing_->samples_dot() + morse_timing_->samples_interword(), transmitter_.level ) );

    dash_interelement_.reset( new C_sparkgap( "dash_interelement_", transmitter_.sparkgap_dash, morse_timing_->samples_dash() + morse_timing_->samples_interelement(), transmitter_.level ) );
    dash_interletter_.reset( new C_sparkgap( "dash_interletter_", transmitter_.sparkgap_dash, morse_timing_->samples_dash() + morse_timing_->samples_interletter(), transmitter_.level ) );
    dash_interword_.reset( new C_sparkgap( "dash_interword_", transmitter_.sparkgap_dash, morse_timing_->samples_dash() + morse_timing_->samples_interword(), transmitter_.level ) );

    bool worked = true;

    worked = worked && dot_interelement_->initialise( output );
    worked = worked && dot_interletter_->initialise( output );
    worked = worked && dot_interword_->initialise( output );

    worked = worked && dash_interelement_->initialise( output );
    worked = worked && dash_interletter_->initialise( output );
    worked = worked && dash_interword_->initialise( output );

    if ( ! worked )
    {
        log_writeln( C_log::LL_ERROR, "C_audio_morse_sparkgap initialisation error" );
    }

    return worked;
}

void
C_audio_morse_sparkgap::select_element_sound( eMorseElement element )
{
    log_writeln_fmt( C_log::LL_VERBOSE_1, "element: %s", to_string( element ) );
    
    eMorseElement element_next = meNone;

    switch ( element )
    {
        case meDit:
        case meDah:
        case meDah2:
        case meDah3:
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
            case meInterElement2:
                sample_source_ = dot_interelement_2_;
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
