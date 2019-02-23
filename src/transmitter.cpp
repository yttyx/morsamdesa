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

// transmitter.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>


#include "audio_morse_cw.h"
#include "audio_morse_double_plate_sounder.h"
#include "audio_morse_sounder.h"
#include "audio_morse_sparkgap.h"
#include "config.h"
#include "log.h"
#include "random.h"
#include "text_to_morse.h"
#include "transmitter.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config  cfg;
extern C_log     log;


C_transmitter::C_transmitter()
{
    default_source_     = 0;
    curr_source_        = 0;
    prev_source_        = 0xffffffff;
}

C_transmitter::~C_transmitter()
{
}

bool
C_transmitter::initialise( const vector< S_transmitter > &  transmitters
                         , shared_ptr< C_audio_output >     output )
{
    for ( unsigned int ii = 0; ii < transmitters.size(); ii++ )
    {
        const S_transmitter transmitter = transmitters[ ii ];

        shared_ptr< C_audio_morse > ptr;

        switch ( transmitter.morse_mode )
        {
            case MM_DOUBLE_PLATE_SOUNDER:
                ptr.reset( new C_audio_morse_double_plate_sounder( transmitter ) );
                break;
            case MM_LED:
                // No action required: for LED output only timing informatin is needed
                break;
            case MM_SOUNDER:
                ptr.reset( new C_audio_morse_sounder( transmitter ) );
                break;
            case MM_SPARKGAP:
                ptr.reset( new C_audio_morse_sparkgap( transmitter ) );
                break;
            default:
                ptr.reset( new C_audio_morse_cw( transmitter, 0 ) );
                break;
        }

        morse_sources_.push_back( ptr  );
    }

    bool worked = true;

    // Set up all Morse audio sources
    for ( unsigned int ii = 0; ii < morse_sources_.size() && worked; ii++ )
    {
        log_writeln_fmt( C_log::LL_INFO, "Initialising transmitter %s", morse_sources_[ ii ]->transmitter_.mnemonic.c_str() );

        worked = worked && morse_sources_[ ii ]->initialise( output );
    }

    find( "DEF", default_source_ );
    
    return worked;
}

void
C_transmitter::select( string & mnemonic )
{
    unsigned int new_source = 0;

    find( mnemonic.c_str(), new_source );

    lock_.lock();
    curr_source_ = new_source;
    lock_.unlock();

    if ( curr_source_ != prev_source_ )
    {
        log_writeln_fmt( C_log::LL_INFO, "Selected transmitter %s for feed %s"
                                       , morse_sources_[ curr_source_ ]->transmitter_.mnemonic.c_str()
                                       , mnemonic.c_str() );
        prev_source_ = curr_source_;
    }
}

const S_transmitter &
C_transmitter::find( const char * mnemonic )
{
    unsigned int source;

    find( mnemonic, source );

    return morse_sources_[ source ]->transmitter();
}

void
C_transmitter::find( const char * mnemonic, unsigned int & source )
{
    source = default_source_;   
   
    // Look up an audio source by mnemonic
    for ( unsigned int ii = 0; ii < morse_sources_.size(); ii++ )
    {
        if ( morse_sources_[ ii ]->transmitter_.mnemonic == mnemonic )
        {
            source = ii;
        }
    }
}

void
C_transmitter::start_sending( string & message, eProsign prosign )
{
    morse_sources_[ curr_source_ ]->start_sending( message, prosign );
}

S_morse_element_state &
C_transmitter::get_send_state()
{
    static S_morse_element_state state;

    state = morse_sources_[ curr_source_ ]->get_send_state();

    return state;
}

void
C_transmitter::set_send_state( S_morse_element_state & state )
{
    morse_sources_[ curr_source_ ]->set_send_state( state );
}

void
C_transmitter::resume_sending()
{
    morse_sources_[ curr_source_ ]->resume_sending();
}

bool
C_transmitter::busy()
{
    return morse_sources_[ curr_source_ ]->busy();
}

void
C_transmitter::interrupt()
{
    morse_sources_[ curr_source_ ]->interrupt();
}

string
C_transmitter::message()
{
    return morse_sources_[ curr_source_ ]->message();
}

void
C_transmitter::write()
{
    lock_.lock();
    morse_sources_[ curr_source_ ]->write();
    lock_.unlock();
}


}
