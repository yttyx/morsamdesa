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

/*! \file audio_command.cpp
    \brief
 */

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "audio_command.h"
#include "log.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log    log;

C_audio_command::C_audio_command( const S_transmitter & transmitter )
{
    sample_source_ = NULL;
    sending_       = false;

    mute_.reset( new C_audio_morse_cw( transmitter, "m", 0 ) );
    unmute_.reset( new C_audio_morse_cw( transmitter, "u", 0 ) );
    interrupt_.reset( new C_audio_morse_cw( transmitter, "i", 0 ) );
    next_.reset( new C_audio_morse_cw( transmitter, "n", 0 ) );
    previous_.reset( new C_audio_morse_cw( transmitter, "p", 0 ) );
    prefix_.reset( new C_audio_morse_cw( transmitter, "s", 0 ) );
}

C_audio_command::~C_audio_command()
{
}

/* /brief Start playback of a sound
 *
 */
void
C_audio_command::trigger( eCommandTone tone )
{
    switch ( tone )
    {
        case ctNone:
            sample_source_ = NULL;
            break;
        case ctMute:
            sample_source_ = mute_;
            break;
        case ctUnmute:
            sample_source_ = unmute_;
            break;
        case ctInterrupt:
            sample_source_ = interrupt_;
            break;
        case ctNext:
            sample_source_ = next_;
            break;
        case ctPrevious:
            sample_source_ = previous_;
            break;
        case ctPrefix:
            sample_source_ = prefix_;
            break;
    }
    if ( sample_source_.get() )
    {
        sample_source_->start_sending();
        sending_ = true;
    }
}

bool
C_audio_command::busy()
{
    return sample_source_->busy();
}

/* /brief Add some sound samples to an output buffer
 *
 * /detail Supply of samples is based around supplying bursts of tick_samples samples or fewer
 *
 */
void
C_audio_command::write()
{
    if ( sample_source_.get() && sample_source_->busy() )
    {
        // We still have samples left to write
        sample_source_->write();
    }
}

bool
C_audio_command::initialise( shared_ptr< C_audio_output > output )
{
    bool worked = true;

    worked = worked && mute_->initialise( output );
    worked = worked && unmute_->initialise( output );
    worked = worked && interrupt_->initialise( output );
    worked = worked && next_->initialise( output );
    worked = worked && previous_->initialise( output );
    worked = worked && prefix_->initialise( output );

    return worked;
}

}
