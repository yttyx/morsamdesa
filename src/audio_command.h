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

// audio_command_H.h

#ifndef audio_command_H
#define audio_command_H

#include "tone.h"

namespace morsamdesa
{

enum eCommandTone
{
    ctNone,
    ctMute,
    ctUnmute,
    ctInterrupt,
    ctNext,
    ctPrevious
};


class C_audio_command
{
public:

    C_audio_command();
    ~C_audio_command();

    void
    trigger( eCommandTone tone );

    bool
    active();

    bool
    initialise( C_audio_output * output );

    bool
    busy() { return sending_; }

    void
    write();

protected:

    void
    reset();

private:

    bool            sending_;

    C_tone          mute_;
    C_tone          unmute_;
    C_tone          interrupt_;
    C_tone          next_;
    C_tone          previous_;
    C_sample_source *sample_source_;
};

}

#endif // audio_command_H
