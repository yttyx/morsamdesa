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

// common.h

#ifndef COMMON_H
#define COMMON_H

#ifndef TRUE
#  define TRUE  (1==1)
#  define FALSE (!TRUE)
#endif

#ifndef NULL
#define NULL 0
#endif

#define NUMBEROF( x ) ( sizeof( x ) / sizeof( *x ) )


const unsigned int SAMPLE_RATE        = 44100;

const double       TONE_AMPLITUDE     = 20000.0;
const double       CW_NOISE_AMPLITUDE = 20000.0;
const double       NOISE_AMPLITUDE    = 10000.0;


#endif  // COMMON_H
