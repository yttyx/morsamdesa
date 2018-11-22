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

// audio_morse_cw.h

#ifndef audio_morse_cw_H
#define audio_morse_cw_H

#include "audio_output.h"
#include "audio_morse.h"
#include "silence.h"
#include "sound_file.h"
#include "text_to_morse.h"
#include "tone.h"


namespace morsamdesa
{

class C_audio_morse_cw : public C_audio_morse
{
public:

    C_audio_morse_cw( C_text_to_morse & text_to_morse );
    virtual ~C_audio_morse_cw();

    virtual bool
    initialise( C_audio_output * output );

protected:


private:

    C_audio_morse_cw();
    
};

}

#endif // audio_morse_cw_H
