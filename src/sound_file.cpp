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

// sound_file.cpp

#include <assert.h>

#include <errno.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"
#include "misc.h"
#include "sound_file.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;

C_sound_file::C_sound_file( const char *description, const string & filename, eSoundMode mode, unsigned int samples, float level )
    : C_sample_source( level )
    , mode_( mode )
    , filename_( filename )
    , description_( description )
{
    active_      = false;
    buffer_      = NULL;
    samples_     = ( samples * 2 ); // * 2 because the sample files have 2 channels, so two file sample represents one 'morsamdesa' sample
    sample_curr_ = 0;
    output_      = NULL;
}

C_sound_file::~C_sound_file()
{
}

/* /brief Add some sound samples to an output buffer
 *
 * /detail Supply of samples is based around supplying bursts of tick_samples samples or fewer
 *
 */
void
C_sound_file::write( bool & samples_exhausted )
{
    samples_exhausted = false;

    if ( active_ )
    {
        for ( ;; )
        {
            // output_->write() returns true if the output buffer is full *after* the sample is added to the buffer
            bool output_buffer_full = output()->write( level_ * buffer_[ sample_curr_++ ] );

            if ( ++sample_curr_ >= samples_ )
            {
                sample_curr_ = 0;

                if ( mode_ == smOneShot )
                {
                    samples_exhausted = true;
                    active_ = false;
                    break;
                }
            }

            if ( output_buffer_full )
            {
                break;
            }
        }
    }
}

/* /brief Allocate a buffer for a sound file's samples and populate it
 *
 * /detail File format must be raw, 2 channel (double mono), 16 bit signed PCM
 *         If samples_ is zero:     the whole sample file will be read in
 *         If samples_ is non-zero: the sample length will be clamped to samples_, null-filling
 *                                  if the file contains fewer samples than samples_
 */
bool
C_sound_file::read()
{
    bool worked = false;

    log_writeln_fmt( C_log::LL_VERBOSE_1, "C_sound_file reading file: %s as %s", filename_.c_str(), description_ );

    long filelength = file_length( filename_ );
    
    if ( filelength > -1 )
    {
        unsigned int samples_in_file = filelength / sizeof( short int );
        unsigned int samples_to_read = 0;

        log_writeln_fmt( C_log::LL_VERBOSE_1, "  Samples in file    : %u", samples_in_file );
        
        // If samples_ is predefined, use that instead of the filename length

        if ( samples_ > 0 )
        {
            log_writeln_fmt( C_log::LL_VERBOSE_1, "  Samples clamped at : %u", samples_ );

            samples_to_read = min( samples_, samples_in_file );
        }
        else
        {
            samples_        = samples_in_file;
            samples_to_read = samples_in_file;
        }

        log_writeln_fmt( C_log::LL_VERBOSE_1, "  Samples to read    : %u", samples_to_read );

        buffer_ = new short int[ samples_ ];

        if ( buffer_ )
        {
            ifstream file_stream( filename_.c_str(), ios::in | ios::binary );

            if ( file_stream.is_open() )
            {
                file_stream.read( ( char * ) buffer_, samples_to_read * sizeof( short int ) );

                if ( ! file_stream.fail() )
                {
                    unsigned int bytes_read = file_stream.gcount();

                    if ( ( bytes_read % sizeof( short int ) == 0 ) )
                    {
                        unsigned int samples_read = bytes_read / sizeof( short int );

                        if ( samples_read < samples_ )
                        {
                            // Pad the sample buffer with silence
                            memset( &buffer_[ samples_read ], 0, ( samples_ - samples_read ) * sizeof( short int ) );

                            log_writeln_fmt( C_log::LL_VERBOSE_1, "  Padding samples    : %u", samples_ - samples_read );
                        }

                        log_writeln( C_log::LL_VERBOSE_1, "Sample file read successful" );
                        worked = true;
                    }
                    else
                    {
                        log_writeln_fmt( C_log::LL_ERROR, "Error: sample bytes read not a multiple of %u", sizeof( short int ) );
                    }
                }
                else
                {
                    log_writeln_fmt( C_log::LL_ERROR, "Error reading sample file %s: %s", filename_.c_str(), strerror( errno ) );
                }

                file_stream.close();
            }
            else
            {
                log_writeln_fmt( C_log::LL_ERROR, "Error opening sample file %s", filename_.c_str() );
            }
        }
        else
        {
            log_writeln_fmt( C_log::LL_ERROR, "Unable to allocate space for sound buffer: %s", strerror( errno ) );
        }
    }
    else
    {
        log_writeln_fmt( C_log::LL_ERROR, "Unable to access sample file %s", filename_.c_str() );
    }

    return worked;
}

}
