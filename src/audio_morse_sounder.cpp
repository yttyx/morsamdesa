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

// audio_morse_sounder.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "audio_morse_sounder.h"
#include "log.h"


using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config  cfg;
extern C_log     log;


C_audio_morse_sounder::C_audio_morse_sounder( const S_transmitter & transmitter )
    : C_audio_morse( transmitter )
{
    sending_      = false;
    interrupt_    = false;
    element_curr_ = meNone;
}

C_audio_morse_sounder::~C_audio_morse_sounder()
{
}

bool
C_audio_morse_sounder::initialise( shared_ptr< C_audio_output > output )
{
    C_audio_morse::initialise();

    dot_.reset( new C_sounder( "dot_", transmitter_.sounder_arm_down, morse_timing_->samples_dot(), transmitter_.level ) );
    dash_.reset( new C_sounder( "dash_", transmitter_.sounder_arm_down, morse_timing_->samples_dash(), transmitter_.level ) );
    dash_2_.reset( new C_sounder( "dash_2", transmitter_.sounder_arm_down, morse_timing_->samples_dash_2(), transmitter_.level ) );
    dash_3_.reset( new C_sounder( "dash_3", transmitter_.sounder_arm_down, morse_timing_->samples_dash_3(), transmitter_.level ) );
    interelement_.reset( new C_sounder( "interelement_", transmitter_.sounder_arm_up, morse_timing_->samples_interelement(), transmitter_.level ) );
    interelement_2_.reset( new C_sounder( "interelement_2", transmitter_.sounder_arm_up, morse_timing_->samples_interelement_2(), transmitter_.level ) );
    interletter_.reset( new C_sounder( "interletter_", transmitter_.sounder_arm_up, morse_timing_->samples_interletter(), transmitter_.level ) );
    interword_.reset( new C_sounder( "interword_", transmitter_.sounder_arm_up, morse_timing_->samples_interword(), transmitter_.level ) );

    bool worked = true;

    worked = worked && dot_->initialise( output );
    worked = worked && dash_->initialise( output );
    worked = worked && dash_2_->initialise( output );
    worked = worked && dash_3_->initialise( output );
    worked = worked && interelement_->initialise( output );
    worked = worked && interelement_2_->initialise( output );
    worked = worked && interletter_->initialise( output );
    worked = worked && interword_->initialise( output );

    if ( ! worked )
    {
        log_writeln( C_log::LL_ERROR, "C_audio_morse_sounder initialisation error" );
    }

    return worked;
}

}
