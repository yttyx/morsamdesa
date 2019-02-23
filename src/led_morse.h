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

// led_morse.h

#ifndef led_morse_H
#define led_morse_H

#include <memory>
#include "config.h"
#include "morse.h"
#include "morse_timing.h"
#include "thread.h"
#include "timer.h"


namespace morsamdesa
{

class C_led_morse : public C_morse, public C_thread
{

public:

    C_led_morse( const S_transmitter & transmitter );
    virtual ~C_led_morse();

    virtual bool
    initialise();

    virtual bool
    start();

    virtual void
    stop();

private:

    void
    thread_handler();

    void
    led_on();

    void
    led_off();

private:

    C_timer         still_here_timer_;
    S_transmitter   transmitter_;
    eMorseElement   element_curr_;

    shared_ptr< C_morse_timing >  morse_timing_;
};

}

#endif // led_morse_H
