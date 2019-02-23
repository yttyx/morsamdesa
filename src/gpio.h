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

// gpio.h

#ifndef gpio_H
#define gpio_H

#include "mutex.h"

#define GPSET0      7
#define GPSET1      8

#define GPCLR0      10
#define GPCLR1      11

#define GPLEV0      13
#define GPLEV1      14

#define GPPUD       37
#define GPPUDCLK0   38
#define GPPUDCLK1   39

#define PI_BANK     ( gpio >> 5 )
#define PI_BIT      ( 1 << ( gpio & 0x1F ) )

// GPIO modes
#define PI_INPUT    0
#define PI_OUTPUT   1
#define PI_ALT0     4
#define PI_ALT1     5
#define PI_ALT2     6
#define PI_ALT3     7
#define PI_ALT4     3
#define PI_ALT5     2

// Values for pull-ups/downs off, pull-down and pull-up
#define PI_PUD_OFF  0
#define PI_PUD_DOWN 1
#define PI_PUD_UP   2

#define GPIO_LED    4
#define GPIO_SENSOR 17

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>


namespace morsamdesa
{

class C_gpio
{
public:

    C_gpio();
    ~C_gpio();

    bool
    initialise();

    void
    write( unsigned gpio, unsigned level );

    bool 
    read( unsigned gpio );

private:

    void
    gpio_set_mode( unsigned gpio_pin, unsigned mode );

    void
    gpio_set_pullupdown( unsigned gpio_pin, unsigned pud );

    unsigned int
    gpio_hardware_revision();

private:

    volatile unsigned int *gpio_reg_;

    unsigned int pi_model_;
    unsigned int pi_revision_;

    C_mutex     gpio_lock_;
};

}

#endif // gpio_H
