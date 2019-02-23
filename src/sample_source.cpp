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

// sample_source.cpp

#include <stdio.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "misc.h"
#include "sample_source.h"


using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;

C_sample_source::C_sample_source( float level )
{
    active_      = false;
    samples_     = 0;
    sample_curr_ = 0;
    level_       = level;
    buffer_      = NULL;
    output_      = NULL;
}

C_sample_source::~C_sample_source()
{
    delete buffer_;
}

/* /brief
 *
 */
void
C_sample_source::dump( const char * desc )
{
    log_writeln( C_log::LL_INFO, "----------------------------------------------------------------------" );
    log_writeln_fmt( C_log::LL_INFO, "Buffer: %s, buffer: %p, length %u", desc, buffer_, samples_ );

    unsigned int col = 0;

    for ( unsigned int sample_count = 0; sample_count < samples_; sample_count++ )
    {
        log_write_fmt( C_log::LL_INFO, "  %5d", buffer_[ sample_count ] );

        if ( ( col++ % 8 ) == 0 )
        {
            log_writeln( C_log::LL_INFO, "" );
        }
    }
    log_writeln( C_log::LL_INFO, "" );
    log_writeln( C_log::LL_INFO, "----------------------------------------------------------------------" );
}

/* /brief
 *
 */
void
C_sample_source::write_to_disk( const char *filename)
{
    FILE *file = fopen( filename, "wb");

    if ( file != NULL )
    {
        long progress = 0;

        for ( unsigned int sample_count = 0; sample_count < samples_;  sample_count++ )
        {
            fwrite( ( void * ) &buffer_[ sample_count ], 1, sizeof( short int ), file );

            if ( ( ++progress % 100 ) == 0 )
            {
                log_writeln_fmt( C_log::LL_ERROR, "%u samples written", sample_count );
            }
        }

        fclose( file );
    }
    else
    {
        log_writeln_fmt( C_log::LL_ERROR, "Error opening file %s", filename );
    }
}

}
