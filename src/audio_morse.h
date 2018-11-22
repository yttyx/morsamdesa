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

// audio_morse.h

#ifndef audio_morse_H
#define audio_morse_H

#include "morse.h"
#include "sample_source.h"


namespace morsamdesa
{

class C_audio_output;

class C_audio_morse : public C_morse
{
public:

    C_audio_morse( C_text_to_morse & text_to_morse );
    virtual ~C_audio_morse();

    virtual bool
    initialise( C_audio_output * output ) = 0;

    virtual void
    write();

protected:

    virtual void
    reset();
    
    void
    select_element_sound();

private:

    C_audio_morse();

protected:

    C_sample_source *dit_;
    C_sample_source *dah_;
    C_sample_source *interelement_;
    C_sample_source *interletter_;
    C_sample_source *interword_;
    C_sample_source *dummy_;

    C_sample_source *sample_source_;

};

}

#endif // audio_morse_H
