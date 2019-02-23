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

// transmitter.h

#ifndef transmitter_H
#define transmitter_H

#include <memory>
#include <vector>

#include "audio_morse.h"
#include "config.h"
#include "mutex.h"


namespace morsamdesa
{

class C_audio_output;

class C_transmitter
{

public:

    C_transmitter();
    virtual ~C_transmitter();

    bool
    initialise( const vector< S_transmitter > & transmitters, shared_ptr< C_audio_output > output );

    void
    select( string & mnemonic );

    const S_transmitter &
    find( const char * mnemonic );

    void
    start_sending( string & message, eProsign prosign );

    S_morse_element_state &
    get_send_state();

    void
    set_send_state( S_morse_element_state & state );

    void
    resume_sending();

    bool
    busy();

    void
    interrupt();

    string
    message();

    void
    write();

protected:

    void
    find( const char * mnemonic, unsigned int & source );

protected:

    vector< shared_ptr< C_audio_morse > > morse_sources_;

    unsigned int            default_source_;
    unsigned int            curr_source_;
    unsigned int            prev_source_;
    C_mutex                 lock_;
};

}

#endif // transmitter_H
