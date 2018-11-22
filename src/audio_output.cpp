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

// audio_output.cpp:

#include <errno.h>
#include <string.h>

#include "audio_output.h"
#include "common.h"
#include "config.h"
#include "log.h"
#include "misc.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;


C_audio_output::C_audio_output()
{
    output_buffer_      = NULL;
    output_buffer_size_ = 0;
    output_buffer_curr_ = 0;
}

C_audio_output::~C_audio_output()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_audio_output destructor" );

    if ( output_buffer_ )
    {
        delete [] output_buffer_;
    }

}

bool
C_audio_output::initialise()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_audio_output::initialise()" );

    output_buffer_size_ = cfg.d().tick_samples;
    output_buffer_      = new short int[ output_buffer_size_ ];

    if ( output_buffer_ == NULL )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Error allocating output buffer: %s", strerror( errno ) );
    }

    return output_buffer_ != NULL;
}

void
C_audio_output::clear_buffer()
{
    memset( output_buffer_, 0, output_buffer_size_ * sizeof( short int ) );
    output_buffer_curr_ = 0;
}

/* /brief Add sample in to output buffer
 *
 * /param [in]  sample Sample to write
 * /return      true if buffer full
 */
bool
C_audio_output::write( short int sample )
{
    if ( output_buffer_curr_ < output_buffer_size_ )
    {
        // Add to existing output buffer sample
        output_buffer_[ output_buffer_curr_++ ] += sample;

        bool buffer_full = output_buffer_curr_ >= output_buffer_size_;

        if ( buffer_full )
        {
            output_buffer_curr_ = 0;
        }

        return buffer_full;
    }
    else
    {
        log_writeln_fmt( C_log::LL_ERROR, "***output_buffer_curr_: %u", output_buffer_curr_ );
    }
    return true;
}

}
