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

// audio_output.h

#ifndef audio_output_H
#define audio_output_H

namespace morsamdesa
{

class C_audio_output
{

public:

    C_audio_output();
    virtual ~C_audio_output();

    virtual bool
    initialise();

    virtual void
    send( bool all = false ) = 0;

    bool
    write( short int sample );

    void
    clear_buffer();

protected:

    short int    *output_buffer_;               // Buffer into which CW and noise samples are combined
    unsigned int output_buffer_size_;           // Capacity of output_buffer_
    unsigned int output_buffer_curr_;           // Current offset into output_buffer_
    
private:


};

}

#endif // audio_output_H
