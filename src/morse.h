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

// morse.h

#ifndef morse_H
#define morse_H

#include "text_to_morse.h"


namespace morsamdesa
{

class C_morse
{
public:

    C_morse();
    virtual ~C_morse();

    virtual bool
    initialise() { return true; }

    virtual bool
    start() { return true; }

    virtual void
    stop() { abort_ = true; }

    virtual void
    reset() {}

    virtual void
    start_sending( string & message, eProsign prosign );

    virtual void
    start_sending() {}

    virtual void
    resume_sending();

    virtual void
    muted( bool muted ) { muted_ = muted; }

    virtual bool
    busy() { return sending_; }

    virtual void
    interrupt() { interrupt_ = true; }

    S_morse_element_state &
    get_send_state();

    void
    set_send_state( S_morse_element_state & state );

    string
    message() { return text_to_morse_->get_message();  }

protected:

    const char *
    to_string( eMorseElement element );

protected:

    bool            sending_;
    bool            interrupt_;
    bool            abort_;
    bool            muted_;
    string          fixed_message_;

    unique_ptr< C_text_to_morse >  text_to_morse_;
};

}

#endif // morse_H
