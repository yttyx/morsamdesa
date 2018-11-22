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

C_audio_command::C_audio_command()
{
    sample_source_ = NULL;
    sending_       = false;
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
            sample_source_ = &mute_;
            break;
        case ctUnmute:
            sample_source_ = &unmute_;
            break;
        case ctInterrupt:
            sample_source_ = &interrupt_;
            break;
        case ctNext:
            sample_source_ = &next_;
            break;
        case ctPrevious:
            sample_source_ = &previous_;
            break;
    }
    if ( sample_source_ )
    {
        sample_source_->trigger();
        sending_ = true;
    }
}

bool
C_audio_command::active()
{
    return sample_source_->active();
}

/* /brief Add some sound samples to an output buffer
 *
 * /detail Supply of samples is based around supplying bursts of tick_samples samples or fewer
 *
 */
void
C_audio_command::write()
{
    if ( sending_ )
    {
        bool samples_exhausted = false;

        sample_source_->write( samples_exhausted );

        if ( samples_exhausted )
        {
            sending_ = false;
        }
    }
}

void
C_audio_command::reset()
{
    mute_.reset();
    unmute_.reset();
    interrupt_.reset();
    next_.reset();
    previous_.reset();
}

bool
C_audio_command::initialise( C_audio_output * output )
{
    bool worked = true;

    worked = worked && mute_.initialise( ttMute, output );
    worked = worked && unmute_.initialise( ttUnmute, output );
    worked = worked && interrupt_.initialise( ttInterrupt, output );
    worked = worked && next_.initialise( ttNext, output );
    worked = worked && previous_.initialise( ttPrevious, output );

    return worked;
}

}
