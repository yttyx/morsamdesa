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


C_audio_morse_sounder::C_audio_morse_sounder( C_text_to_morse & text_to_morse )
    : C_audio_morse( text_to_morse )
{
    sending_      = false;
    interrupt_    = false;
    element_curr_ = meNone;
}

C_audio_morse_sounder::~C_audio_morse_sounder()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_audio_morse_sounder destructor" );
}

bool
C_audio_morse_sounder::initialise( C_audio_output * output )
{
    dit_          = new C_sounder( cfg.c().morse_sounder_arm_down, cfg.d().dit_samples );
    dah_          = new C_sounder( cfg.c().morse_sounder_arm_down, cfg.d().dah_samples );
    interelement_ = new C_sounder( cfg.c().morse_sounder_arm_up,   cfg.d().interelement_samples );
    interletter_  = new C_sounder( cfg.c().morse_sounder_arm_up,   cfg.d().interletter_samples );
    interword_    = new C_sounder( cfg.c().morse_sounder_arm_up,   cfg.d().interword_samples );

    bool worked = true;

    worked = worked && dit_->initialise( output );
    worked = worked && dah_->initialise( output );
    worked = worked && interelement_->initialise( output );
    worked = worked && interletter_->initialise( output );
    worked = worked && interword_->initialise( output );

    return worked;
}

}
