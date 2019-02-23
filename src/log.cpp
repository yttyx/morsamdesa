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

// log.cpp
//

#include <assert.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "log.h"

using namespace  morsamdesa;

namespace morsamdesa
{

C_log log;


C_log::C_log()
{
    level_    = LL_INFO;
    datetime_ = false;
    fileline_ = false;
}

C_log::~C_log()
{
}

void
C_log::initialise( int level, bool datetime )
{
    level_    = ( eLogLevel ) level;
    datetime_ = datetime;

    if ( level_ >= LL_VERBOSE_3 )
    {
        fileline_ = true;
    }
}

void
C_log::write_line( eLogLevel level, const char *file, int line, bool newline, const char *format, ... )
{
    assert( format );

    if ( level <= level_ )
    {
        log_lock_.lock();

        va_list arg_ptr;

        va_start( arg_ptr, format );

        string line_str = format_string_internal( fileline_, file, line, format, arg_ptr );

        if ( newline )
        {
            line_str += "\n";
        }

        if ( datetime_ )
        {
            string datetime = get_datetime();
            
            fprintf( stdout, "%s %s", datetime.c_str(), line_str.c_str() );
        }
        else
        {
            fprintf( stdout, "%s", line_str.c_str() );
        }
        
        fflush( stdout );

        log_lock_.unlock();
    }
}

string
C_log::format_string( const char *format, ... )
{
    assert( format );

    va_list arg_ptr;
    
    va_start( arg_ptr, format );

    string str = format_string_internal( false, "", 0, format, arg_ptr );

    return str;
}

string
C_log::format_string_internal( bool fileline, const char *file, int line, const char *format, va_list arg_ptr )
{
    assert( format );

    string fileline_str;
    
    if ( fileline )
    {
        fileline_str = format_string( "%s %d ", file, line );
    }

    char buf[ 4096 + 1 ];

    vsnprintf( buf, sizeof( buf ) - 1, format, arg_ptr );
    va_end( arg_ptr );

    return fileline_str + string( buf );
}

string
C_log::get_datetime()
{
    time_t    now = time( 0 );
    struct tm tms = *localtime( &now );

    string datetime = format_string( "%02d/%02d/%04d %02d:%02d:%02d ", tms.tm_mday, tms.tm_mon + 1, tms.tm_year + 1900,
                                                                       tms.tm_hour, tms.tm_min,     tms.tm_sec );
    return datetime;
}

}
