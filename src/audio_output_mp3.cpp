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

// audio_output_mp3.cpp:

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "misc.h"
#include "audio_output_mp3.h"

using namespace  morsamdesa;

namespace morsamdesa
{
extern C_config cfg;
extern C_log    log;


C_audio_output_mp3::C_audio_output_mp3()
{
    lame_                   = NULL;
    file_                   = NULL;
    id3_tags_added_         = false;

    mp3_input_buffer_       = NULL;
    mp3_input_buffer_size_  = 0;
    mp3_input_buffer_curr_  = 0;

    mp3_output_buffer_      = NULL;
    mp3_output_buffer_size_ = 0;
}

C_audio_output_mp3::~C_audio_output_mp3()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_audio_output_mp3 destructor" );

    if ( lame_ )
    {
        lame_close( lame_ );
    }

    if ( file_ )
    {
        fclose( file_ );
    }

    if ( mp3_input_buffer_ != NULL )
    {
        delete [] mp3_input_buffer_;
    }
    
    if ( mp3_output_buffer_ != NULL )
    {
        delete [] mp3_output_buffer_;
    }
}

bool
C_audio_output_mp3::initialise()
{
    bool worked = C_audio_output::initialise();

    file_ = fopen( cfg.c().output_file.c_str(), "wb" );

    if ( file_ == NULL )
    {
        log_writeln_fmt( C_log::LL_ERROR, "**Error %d creating file '%s'", errno, cfg.c().output_file.c_str() );
    }

    worked = worked && file_;
    worked = worked && init_lame();
    worked = worked && init_buffers();

    return worked;
}

/*
 * Convert samples to MP3 format (with buffering)
 */
void
C_audio_output_mp3::send( bool all )
{
    memcpy( &mp3_input_buffer_[ mp3_input_buffer_curr_ ], output_buffer_, output_buffer_size_ * sizeof( short int) );
    mp3_input_buffer_curr_ += output_buffer_size_;

    if ( ( ( mp3_input_buffer_curr_ + output_buffer_size_ ) > mp3_input_buffer_size_ ) || all )
    {
        encode_mp3( all ? EM_ENCODE_FLUSH : EM_ENCODE );
    }
}

bool
C_audio_output_mp3::init_lame()
{
    lame_ = lame_init();
    
    lame_set_num_channels( lame_, 1 );
    lame_set_brate( lame_, 192 );                       // bit rate
    lame_set_in_samplerate( lame_, SAMPLE_RATE );
    lame_set_out_samplerate( lame_, SAMPLE_RATE );
    lame_set_mode( lame_, JOINT_STEREO );
    lame_set_quality(lame_, 5 );                        // 0: best; 9: worst

    id3tag_init( lame_ );
    id3tag_set_artist( lame_, "morsamdesa" );
    
    struct tm time;
    
    current_time( time );

    id3tag_set_year( lame_, C_log::format_string( "%04d", time.tm_year + 1900 ).c_str() );
    id3tag_set_title( lame_, "morsamdesa" );
    id3tag_set_album( lame_, "morsamdesa" );
    id3tag_set_comment( lame_, "morsamdesa-generated file" );

    bool worked = ( lame_init_params( lame_ ) >= 0 );

    if ( ! worked )
    {
        log_writeln( C_log::LL_ERROR, "**LAME initialisation failed" );
    }

    return worked;
}

bool
C_audio_output_mp3::init_buffers()
{
    // From https://github.com/gypified/libmp3lame/blob/master/API
    // Rule of thumb for calculating buffer sizes:
    // mp3buffer_size (in bytes) = ( 1.25 * number_of_samples ) + 7200.

    mp3_input_buffer_size_ = 5 * 50 * cfg.d().tick_samples;                                                 // Enough space for 5 seconds' sound
                                                                                                            // One tick is 20 mS i.e. 1/50 second
    mp3_output_buffer_size_ = ( unsigned int )( ( 1.25 * ( float ) mp3_input_buffer_size_ ) + 7200.0  );    //

    mp3_input_buffer_  = new short int [ mp3_input_buffer_size_ ];
    mp3_output_buffer_ = new unsigned char [ mp3_output_buffer_size_ ];

    if ( ( mp3_input_buffer_ == NULL ) || ( mp3_output_buffer_ == NULL ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "** C_audio_output_mp3::init_buffers(): new[] failed: %s", strerror( errno ) );
        return false;
    }

    return true;
}

void
C_audio_output_mp3::encode_mp3( eEncodeMode mode )
{
    unsigned int mp3_bytes = 0;

    if ( mp3_input_buffer_curr_ > 0 )
    {
        mp3_bytes = lame_encode_buffer( lame_, mp3_input_buffer_, mp3_input_buffer_, mp3_input_buffer_curr_, mp3_output_buffer_, mp3_output_buffer_size_ );
        mp3_input_buffer_curr_ = 0;

        log_writeln_fmt( C_log::LL_VERBOSE_3, "mp3:  %u bytes generated", mp3_bytes );
    }
    
    if ( mode == EM_ENCODE_FLUSH )
    {
        mp3_bytes = lame_encode_flush( lame_, mp3_output_buffer_, mp3_output_buffer_size_ );

        log_writeln_fmt( C_log::LL_VERBOSE_3, "mp3: %u bytes generated (flush)", mp3_bytes );
    }

    if ( mp3_bytes > 0 )
    {
        if ( fwrite( mp3_output_buffer_, sizeof( char ), mp3_bytes, file_ ) != mp3_bytes )
        {
            log_writeln_fmt( C_log::LL_ERROR, "**Error: Writing %d bytes to file failed", mp3_bytes );
        }
        else
        {
            log_writeln_fmt( C_log::LL_INFO, "mp3: wrote %d bytes", mp3_bytes );
        }
    }
    else if ( mp3_bytes < 0 )
    {
        log_writeln_fmt( C_log::LL_ERROR, "**Error: lame_encode_buffer returned %d", mp3_bytes );
    }
}

}
