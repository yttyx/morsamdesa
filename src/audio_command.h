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

// audio_command_H.h

#ifndef audio_command_H
#define audio_command_H

#include <memory>

#include "audio_morse_cw.h"

namespace morsamdesa
{

enum eCommandTone
{
    ctNone,
    ctMute,
    ctUnmute,
    ctInterrupt,
    ctNext,
    ctPrevious,
    ctPrefix
};


class C_audio_command
{
public:

    C_audio_command( const S_transmitter & transmitter );
    ~C_audio_command();

    void
    trigger( eCommandTone tone );

    bool
    busy();

    bool
    initialise( shared_ptr< C_audio_output > output );

    void
    write();

private:

    bool    sending_;

    shared_ptr< C_audio_morse_cw > mute_;
    shared_ptr< C_audio_morse_cw > unmute_;
    shared_ptr< C_audio_morse_cw > interrupt_;
    shared_ptr< C_audio_morse_cw > next_;
    shared_ptr< C_audio_morse_cw > previous_;
    shared_ptr< C_audio_morse_cw > prefix_;

    shared_ptr< C_audio_morse_cw > sample_source_;
};

}

#endif // audio_command_H
