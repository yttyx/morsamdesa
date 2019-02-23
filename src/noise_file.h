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

// noise_file.h

#ifndef C_noise_file_H
#define C_noise_file_H

#include <memory>

#include "sound_file.h"


namespace morsamdesa
{

enum eNoiseState
{
    nsFadeUp,
    nsFadeDown,
    nsUnchanging
};


class C_noise_file : public C_sound_file
{

public:

    C_noise_file( const string & filename, eSoundMode mode );
    ~C_noise_file();

    bool
    initialise( shared_ptr< C_audio_output > output );

    void
    write();

    void
    trigger( eNoiseState state );

    eNoiseState
    state() { return state_; }

protected:

    void
    calculate_noise_envelopes();

    virtual bool
    read();

private:

    double       *bg_noise_fade_rising_edge_;
    double       *bg_noise_fade_falling_edge_;
    
    double       bg_noise_level_current_;
    unsigned int bg_noise_edge_curr_;

    eNoiseState  state_;
};

}

#endif // C_noise_file_H
