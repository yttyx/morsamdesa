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

// audio_morse_cw.h

#ifndef audio_morse_cw_H
#define audio_morse_cw_H

#include "audio_output.h"
#include "audio_morse.h"


namespace morsamdesa
{

class C_audio_morse_cw : public C_audio_morse
{
public:

    C_audio_morse_cw( const S_transmitter & transmitter, int frequency_shift );
    C_audio_morse_cw( const S_transmitter & transmitter, string fixed_message, int frequency_shift );
    virtual ~C_audio_morse_cw();

    virtual bool
    initialise( shared_ptr< C_audio_output > output );

    virtual void
    start_sending();

protected:

    int frequency_shift_;           // Number of semitones to shift the fundamental frequency

};

}

#endif // audio_morse_cw_H
