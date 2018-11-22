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

// audio_morse_sounder.h

#ifndef audio_morse_sounder_H
#define audio_morse_sounder_H

#include "audio_morse.h"
#include "sounder.h"
#include "sound_file.h"
#include "text_to_morse.h"
#include "tone.h"


namespace morsamdesa
{

class C_audio_morse_sounder : public C_audio_morse
{
public:

    C_audio_morse_sounder( C_text_to_morse & text_to_morse );
    virtual ~C_audio_morse_sounder();

    virtual bool
    initialise( C_audio_output * output );

private:

    C_audio_morse_sounder();

};

}

#endif // audio_morse_sounder_H
