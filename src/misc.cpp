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

/*! \file misc.cpp
    \brief Miscellaneous support functions
*/

#include <assert.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "misc.h"
#include "config.h"
#include "mutex.h"


namespace morsamdesa
{

long
file_length( const string & filename )
{
    struct stat stat_buf;

    int rc = stat( filename.c_str(), &stat_buf );

    return rc == 0 ? stat_buf.st_size : -1;
}

/** \brief Get current time (HH:MM)

    @param[out]     tm_struct: Time structure populated
*/
void
current_time( tm & tm_struct )
{
    time_t  now = time( 0 );

    tm_struct = *localtime( &now );
}

/** \brief Format time

    @param          tm_struct: Time structure
    @return         Time as string, format HH MM
*/
string
format_time_hh_mm( const tm & tm_struct )
{
    char       buf[ 80 ];

    strftime( buf, sizeof( buf ), "%H %M", &tm_struct );

    return buf;
}

string
regex_error( int regex_res, const regex_t & reg )
{
    char buf[ 4096 ];

    regerror( regex_res, &reg, buf, sizeof( buf ) );

    return string( buf );
}

void
rtrim_string( string & s )
{
    s.erase( s.find_last_not_of( " \n\r\t" ) + 1 );
}

void
delay( unsigned int milliseconds)
{
    usleep( 1000 * milliseconds );
}

}
