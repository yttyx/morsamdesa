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

// audio_morse_double_plate_sounder.h

#ifndef audio_morse_double_plate_sounder_H
#define audio_morse_double_plate_sounder_H

#include "audio_morse.h"
#include "cw.h"
#include "sounder.h"
#include "sound_file.h"


namespace morsamdesa
{

class C_audio_morse_double_plate_sounder : public C_audio_morse
{
public:

    C_audio_morse_double_plate_sounder( const S_transmitter & transmitter );
    virtual ~C_audio_morse_double_plate_sounder();

    virtual bool
    initialise( shared_ptr< C_audio_output > );

protected:

    virtual void
    select_element_sound( eMorseElement element );

};

}

#endif // audio_morse_double_plate_sounder_H
