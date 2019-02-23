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

// audio_morse_cw.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "audio_morse_cw.h"
#include "cw.h"
#include "silence.h"
#include "log.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config  cfg;
extern C_log     log;

C_audio_morse_cw::C_audio_morse_cw( const S_transmitter & transmitter, int frequency_shift )
    : C_audio_morse( transmitter )
{
    frequency_shift_ = frequency_shift;
}

C_audio_morse_cw::C_audio_morse_cw( const S_transmitter & transmitter, string fixed_message, int frequency_shift )
    : C_audio_morse( transmitter )
{
    fixed_message_   = fixed_message;
    frequency_shift_ = frequency_shift;
}

C_audio_morse_cw::~C_audio_morse_cw()
{
}

bool
C_audio_morse_cw::initialise( shared_ptr< C_audio_output > output )
{
    C_audio_morse::initialise();

    dot_.reset( new C_cw( transmitter_ ) );
    dash_.reset( new C_cw( transmitter_ ) );
    dash_2_.reset( new C_cw( transmitter_ ) );
    dash_3_.reset( new C_cw( transmitter_ ) );

    interelement_.reset( new C_silence() );
    interelement_2_.reset( new C_silence() );
    interletter_.reset( new C_silence() );
    interword_.reset( new C_silence() );

    bool worked = true;

    worked = worked && dot_->initialise( morse_timing_->samples_dot(), output, frequency_shift_ );
    worked = worked && dash_->initialise( morse_timing_->samples_dash(), output, frequency_shift_ );
    worked = worked && dash_2_->initialise( morse_timing_->samples_dash_2(), output, frequency_shift_ );
    worked = worked && dash_3_->initialise( morse_timing_->samples_dash_3(), output, frequency_shift_ );
    worked = worked && interelement_->initialise( morse_timing_->samples_interelement(), output );
    worked = worked && interelement_2_->initialise( morse_timing_->samples_interelement_2(), output );
    worked = worked && interletter_->initialise( morse_timing_->samples_interletter(), output );
    worked = worked && interword_->initialise( morse_timing_->samples_interword(), output );

    if ( ! worked )
    {
        log_writeln( C_log::LL_ERROR, "C_audio_morse_cw initialisation error" );
    }

    return worked;
}

void
C_audio_morse_cw::start_sending()
{
    C_morse::start_sending( fixed_message_, psNone );
}

}
