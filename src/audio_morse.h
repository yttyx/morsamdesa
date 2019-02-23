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

// audio_morse.h

#ifndef audio_morse_H
#define audio_morse_H

#include <memory>

#include "config.h"
#include "morse.h"
#include "morse_timing.h"
#include "sample_source.h"


namespace morsamdesa
{

class C_audio_output;

class C_audio_morse : public C_morse
{
    friend class C_transmitter;

public:

    C_audio_morse( const S_transmitter & transmitter );
    virtual ~C_audio_morse();

    virtual bool
    initialise( shared_ptr< C_audio_output > output ) = 0;

    virtual void
    write();

    const S_transmitter & transmitter() { return transmitter_; }

protected:

    virtual bool
    initialise();

    virtual void
    reset();

    void
    reset( shared_ptr< C_sample_source > & sound );

    virtual void
    select_element_sound( eMorseElement element );

private:

    C_audio_morse(){}

protected:

    shared_ptr< C_sample_source > dot_;
    shared_ptr< C_sample_source > dot_interelement_;         // dot plus inter-element silence
    shared_ptr< C_sample_source > dot_interelement_2_;       // dot plus long inter-element silence (American Morse only)
    shared_ptr< C_sample_source > dot_interletter_;          // dot plus inter-letter silence
    shared_ptr< C_sample_source > dot_interword_;            // dot plus inter-word silence

    shared_ptr< C_sample_source > dash_;
    shared_ptr< C_sample_source > dash_2_;                    // long dash used in 'L' (American Morse only)
    shared_ptr< C_sample_source > dash_3_;                    // long dash used in '0' (American Morse only)
    shared_ptr< C_sample_source > dash_interelement_;         // dash plus inter-element silence
    shared_ptr< C_sample_source > dash_interletter_;          // dash plus inter-letter silence
    shared_ptr< C_sample_source > dash_interword_;            // dash plus inter-word silence

    shared_ptr< C_sample_source > interelement_;             // inter-element silence
    shared_ptr< C_sample_source > interelement_2_;           // long inter-element silence  (American Morse only)
    shared_ptr< C_sample_source > interletter_;              // inter-letter silence
    shared_ptr< C_sample_source > interword_;                // inter-word silence

    shared_ptr< C_sample_source > dummy_;
    shared_ptr< C_sample_source > sample_source_;

    shared_ptr< C_morse_timing >  morse_timing_;

    S_transmitter transmitter_;
    eMorseElement element_curr_;
};

}

#endif // audio_morse_H
