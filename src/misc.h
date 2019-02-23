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

// misc.h

#ifndef MISC_H
#define MISC_H

#include <regex.h>
#include <string>

using namespace std;

namespace morsamdesa
{

long
file_length( const string & filename );

void
current_time(  tm & tm_struct );

string
format_time_hh_mm( const tm & tm_struct );

string
regex_error( int regex_res, const regex_t & reg );

void
rtrim_string( string & s );

void
delay( unsigned int milliseconds);

}

#endif // MISC_H
