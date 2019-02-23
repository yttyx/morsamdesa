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

// sample_source.h

#ifndef C_sample_source_H
#define C_sample_source_H

#include <memory>

#include "audio_output.h"
#include "config.h"

namespace morsamdesa
{

extern C_config cfg;

class C_sample_source
{

public:

    C_sample_source( float level );
    virtual ~C_sample_source();

    virtual bool
    initialise( shared_ptr< C_audio_output > output ) { return false; }

    virtual bool
    initialise( unsigned int samples, shared_ptr< C_audio_output >, int frequency_shift ) { return false; }

    virtual bool
    initialise( unsigned int samples, shared_ptr< C_audio_output > ) { return false; }

    virtual void
    write( bool & samples_exhausted ) {}

    void
    dump( const char * desc );

    void
    write_to_disk( const char *filename );

    shared_ptr< C_audio_output >
    output() { return output_; }

    virtual void
    reset() { sample_curr_ = 0; }

    void
    trigger() { reset(); active_ = true; }

    bool
    active() { return active_; }

protected:

    bool         active_;       // Currently supplying samples
    
    float        level_;

    short int    *buffer_;      // Sample buffer
    unsigned int samples_;      // Number of samples in buffer_
    unsigned int sample_curr_;  // Current sample

    shared_ptr< C_audio_output > output_;
};

}

#endif  // C_sample_source_H
