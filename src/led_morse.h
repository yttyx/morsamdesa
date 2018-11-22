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

// led_morse.h

#ifndef led_morse_H
#define led_morse_H

#include "morse.h"
#include "text_to_morse.h"
#include "thread.h"
#include "timer.h"


namespace morsamdesa
{

class C_led_morse : public C_morse, public C_thread
{

public:

    C_led_morse( C_text_to_morse & text_to_morse );
    virtual ~C_led_morse();

    virtual bool
    initialise();

    virtual bool
    start();

    virtual void
    stop();

private:

    C_led_morse();

    void
    thread_handler();

    void
    led_on();

    void
    led_off();

    int
    gpio_initialise();

    void
    gpio_set_mode( unsigned gpio, unsigned mode );

    void
    gpio_set_pullupdown( unsigned gpio, unsigned pud );

    void
    gpio_write( unsigned gpio, unsigned level );

    unsigned int
    gpio_hardware_revision();

private:

    volatile unsigned int *gpio_reg_;

    unsigned int pi_model_;
    unsigned int pi_revision_;

    C_timer      still_here_timer_;

};

}

#endif // led_morse_H
