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

// morse.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "morse.h"
#include "log.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log     log;


struct element_type
{
    const char    *text;
    eMorseElement element;
};

static element_type element_types[] =
{
    { "None",           meNone              },
    { "Dit",            meDit               },
    { "Dah",            meDah               },
    { "InterElement",   meInterElement      },
    { "InterCharacter", meInterCharacter    },
    { "InterWord",      meInterWord         },
    { "InterMessage",   meEndOfMessage      },
    { NULL,             meInvalid           }
};

C_morse::C_morse(  C_text_to_morse & text_to_morse )
    : text_to_morse_( text_to_morse )
{
    sending_       = false;
    interrupt_     = false;
    abort_         = false;

    element_curr_  = meNone;
}

C_morse::~C_morse()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_morse destructor" );
}

const char *
C_morse::to_string( eMorseElement element )
{
    for ( element_type *t = element_types; t->text; t++ )
    {
        if ( t->element  == element )
        {
            return t->text;
        }
    }

    return "?";
}

}
