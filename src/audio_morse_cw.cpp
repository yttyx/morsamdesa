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

// audio_morse_cw.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "audio_morse_cw.h"
#include "log.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config  cfg;
extern C_log     log;


C_audio_morse_cw::C_audio_morse_cw( C_text_to_morse & text_to_morse )
    : C_audio_morse( text_to_morse )
{
}

C_audio_morse_cw::~C_audio_morse_cw()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_audio_morse_cw destructor" );
}

bool
C_audio_morse_cw::initialise( C_audio_output * output )
{
    dit_            = new C_tone();
    dah_            = new C_tone();
    interelement_   = new C_silence();
    interletter_    = new C_silence();
    interword_      = new C_silence();

    bool worked = true;

    worked = worked && dit_->initialise( ttDit, output );
    worked = worked && dah_->initialise( ttDah, output );

    worked = worked && interelement_->initialise( cfg.d().interelement_samples, output );
    worked = worked && interletter_->initialise( cfg.d().interletter_samples,   output );
    worked = worked && interword_->initialise( cfg.d().interword_samples,       output );

    return worked;
}


}
