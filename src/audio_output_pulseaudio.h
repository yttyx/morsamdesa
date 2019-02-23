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

// audio_output_pulseaudio.h
#ifndef audio_output_pulseaudio_H
#define audio_output_pulseaudio_H

#include <pulse/simple.h>
#include <pulse/error.h>

#include "audio_output.h"

namespace morsamdesa
{

class C_audio_output_pulseaudio : public C_audio_output
{

public:

    C_audio_output_pulseaudio();
    virtual ~C_audio_output_pulseaudio();

    virtual bool
    initialise();

protected:

    virtual void
    send( bool all );

private:

    pa_simple   *pa_handle_;

};

}

#endif  // audio_output_pulseaudio_H
