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
    { "Dah2",           meDah2              },
    { "Dah3",           meDah3              },
    { "InterElement",   meInterElement      },
    { "InterElement2",  meInterElement2     },
    { "InterCharacter", meInterCharacter    },
    { "InterWord",      meInterWord         },
    { "EndOfMessage",   meEndOfMessage      },
    { NULL,             meInvalid           }
};

C_morse::C_morse()
{
    sending_       = false;
    interrupt_     = false;
    abort_         = false;
}

C_morse::~C_morse()
{
}

void
C_morse::start_sending( string & message, eProsign prosign )
{
    sending_ = false;

    text_to_morse_->convert( message, prosign );

    resume_sending();
}

void
C_morse::resume_sending()
{
    reset();
    
    sending_ = true;
}

S_morse_element_state &
C_morse::get_send_state()
{
    return text_to_morse_->state_;
}

void
C_morse::set_send_state( S_morse_element_state & state )
{
    text_to_morse_->state_ = state;
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
