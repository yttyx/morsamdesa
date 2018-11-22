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

// morse.h

#ifndef morse_H
#define morse_H

#include "text_to_morse.h"


namespace morsamdesa
{

class C_morse
{
public:

    C_morse( C_text_to_morse & text_to_morse );
    virtual ~C_morse();

    virtual bool
    initialise() { return true; }

    virtual bool
    start() { return true; }

    virtual void
    stop() { abort_ = true; }

    virtual void
    start_sending() { sending_ = true; }

    virtual void
    muted( bool muted ) { muted_ = muted; }

    virtual bool
    busy() { return sending_; }

    virtual void
    interrupt() { interrupt_ = true; }

protected:

    const char *
    to_string( eMorseElement element );

private:

    C_morse();

protected:

    bool            sending_;
    bool            interrupt_;
    bool            abort_;
    bool            muted_;

    C_text_to_morse & text_to_morse_;

    eMorseElement   element_curr_;

};

}

#endif // morse_H
