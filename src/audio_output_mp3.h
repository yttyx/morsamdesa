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

// audio_output_mp3.h
#ifndef audio_output_mp3_H
#define audio_output_mp3_H

#include "lame.h"

#include "audio_output.h"

namespace morsamdesa
{

class C_audio_output_mp3 : public C_audio_output
{
public:

    C_audio_output_mp3();
    virtual ~C_audio_output_mp3();

    virtual bool
    initialise();

    virtual void
    send( bool all );

protected:

    enum eEncodeMode
    {
        EM_ENCODE,
        EM_ENCODE_FLUSH
    };

    bool
    init_lame();

    bool
    init_buffers();

    void
    encode_mp3( eEncodeMode mode );

private:

    lame_global_flags   *lame_;
    FILE                *file_;

    short int           *mp3_input_buffer_;         // Buffer for accumulating samples (input to MP3 conversion)
    unsigned int        mp3_input_buffer_size_;     //
    unsigned int        mp3_input_buffer_curr_;

    unsigned char       *mp3_output_buffer_;        // Buffer for LAME encoded data (output of MP3 conversion)
    unsigned int        mp3_output_buffer_size_;    //

    bool                id3_tags_added_;
};

}

#endif  // audio_output_mp3_H
