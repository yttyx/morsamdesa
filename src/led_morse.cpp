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

// led_morse.cpp
//
// NB: See https://raspberrypi.stackexchange.com/questions/40105/access-gpio-pins-without-root-no-access-to-dev-mem-try-running-as-root


#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "led_morse.h"
#include "config.h"
#include "gpio.h"
#include "log.h"
#include "misc.h"


using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config  cfg;
extern C_gpio    gpio;
extern C_log     log;


C_led_morse::C_led_morse( const S_transmitter & transmitter )
    : transmitter_( transmitter )
{
    element_curr_ = meNone;
}

C_led_morse::~C_led_morse()
{
}

// -----------------------------------------------------------------------------------
// Foreground thread code
// -----------------------------------------------------------------------------------

bool
C_led_morse::initialise()
{
    led_off();

    morse_timing_.reset( new C_morse_timing( transmitter_.speed_char
                       , transmitter_.speed_effective
                       , transmitter_.interval_multiplier_char
                       , transmitter_.interval_multiplier_word
                       , transmitter_.morse_mode
                       , transmitter_.morse_code) );

    text_to_morse_.reset( new C_text_to_morse( transmitter_.alphanumeric_only
                                             , transmitter_.morse_code ) );
    return true;
}

bool
C_led_morse::start()
{
    return thread_start();
}

void
C_led_morse::stop()
{
    C_morse::stop();

    thread_await_exit();
}

// -----------------------------------------------------------------------------------
// Background thread code
// -----------------------------------------------------------------------------------
void
C_led_morse::thread_handler()
{
    while ( ! abort_ )
    {
        while ( sending_ && ( ! interrupt_ ) && ( ! abort_ ) && ( element_curr_ != meEndOfMessage ) )
        {
            element_curr_ = text_to_morse_->get_element();

            switch ( element_curr_ )
            {
                case meDit:
                    led_on();
                    delay( morse_timing_->duration_dot() );
                    led_off();
                    break;
                case meDah:
                    led_on();
                    delay( morse_timing_->duration_dash() );
                    led_off();
                    break;
                case meDah2:
                    led_on();
                    delay( morse_timing_->duration_dash_2() );
                    led_off();
                    break;
                case meDah3:
                    led_on();
                    delay( morse_timing_->duration_dash_3() );
                    led_off();
                    break;
                case meInterElement:
                    delay( morse_timing_->duration_element() );
                    break;
                case meInterElement2:
                    delay( morse_timing_->duration_interelement_2() );
                    break;
                case meInterCharacter:
                    delay( morse_timing_->duration_interletter() );
                    break;
                case meInterWord:
                    delay( morse_timing_->duration_interword() );
                    break;
                case meEndOfMessage:
                    sending_ = false;
                    break;
                default:
                    break;
            }
        }

        interrupt_      = false;
        sending_        = false;
        element_curr_   = meNone;

        // While waiting for something to send, flash the LED every 5 seconds to show we're still here

        if ( ! still_here_timer_.active() )
        {
            still_here_timer_.start( 5000 );
        }
        else
        {
            if ( still_here_timer_.expired() )
            {
                if ( muted_ )
                {
                    led_on();
                    delay( morse_timing_->duration_dot() );
                    led_off();
                }
                else
                {
                    led_on();
                    delay( morse_timing_->duration_dot() );
                    led_off();

                    delay( morse_timing_->duration_interelement() );

                    led_on();
                    delay( morse_timing_->duration_dot() );
                    led_off();
                }
            }
        }

        delay( 40 );
    }
}

void
C_led_morse::led_on()
{
    gpio.write( GPIO_LED, 1 );
}

void
C_led_morse::led_off()
{
    gpio.write( GPIO_LED, 0 );
}

}
