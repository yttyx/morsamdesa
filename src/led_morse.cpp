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
#include "log.h"
#include "misc.h"

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

/* gpio modes. */

#define PI_INPUT    0
#define PI_OUTPUT   1
#define PI_ALT0     4
#define PI_ALT1     5
#define PI_ALT2     6
#define PI_ALT3     7
#define PI_ALT4     3
#define PI_ALT5     2

/* Values for pull-ups/downs off, pull-down and pull-up. */

#define PI_PUD_OFF  0
#define PI_PUD_DOWN 1
#define PI_PUD_UP   2


using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config  cfg;
extern C_log     log;


C_led_morse::C_led_morse( C_text_to_morse & text_to_morse )
    : C_morse( text_to_morse )
{
    gpio_reg_       = ( unsigned int * ) MAP_FAILED;
    pi_model_       = 0;
    pi_revision_    = 0;
}

C_led_morse::~C_led_morse()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_led_morse destructor" );
}

// -----------------------------------------------------------------------------------
// Foreground thread code
// -----------------------------------------------------------------------------------

bool
C_led_morse::initialise()
{
    if ( gpio_initialise() < 0 )
    {
        log_writeln( C_log::LL_ERROR, "gpio_initialise failed" );
        return false;
    }

    gpio_set_pullupdown( 4, PI_PUD_UP );
    gpio_set_mode( 4, PI_OUTPUT );
    
    led_off();
    
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
            element_curr_ = text_to_morse_.get_element();

            switch ( element_curr_ )
            {
                case meDit:
                    led_on();
                    delay( cfg.d().led_duration_dot );
                    led_off();
                    break;
                case meDah:
                    led_on();
                    delay( cfg.d().led_duration_dash );
                    led_off();
                    break;
                case meInterElement:
                    delay( cfg.d().led_duration_element );
                    break;
                case meInterCharacter:
                    delay( cfg.d().led_duration_interletter );
                    break;
                case meInterWord:
                    delay( cfg.d().led_duration_interword );
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
                    delay( cfg.d().led_duration_dot );
                    led_off();
                }
                else
                {
                    led_on();
                    delay( cfg.d().led_duration_dot );
                    led_off();

                    delay( cfg.d().led_duration_interelement );

                    led_on();
                    delay( cfg.d().led_duration_dot );
                    led_off();
                }
            }
        }

        delay( cfg.d().duration_tick );
    }
}

void
C_led_morse::led_on()
{
    gpio_write( 4, 1 );
}

void
C_led_morse::led_off()
{
    gpio_write( 4, 0 );
}

int
C_led_morse::gpio_initialise()
{
    // Set pi_model and pi_revision_
    pi_revision_ = gpio_hardware_revision();

    int fd = open( "/dev/gpiomem", O_RDWR | O_SYNC );

    if ( fd < 0 )
    {
        log_writeln( C_log::LL_ERROR, "Failed to open /dev/gpiomem" );
        return -1;
    }

    gpio_reg_ = ( unsigned int * ) mmap( NULL, 0xB4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

    close( fd );

    if ( gpio_reg_ == MAP_FAILED )
    {
        log_writeln( C_log::LL_ERROR, "Error, Bad, mmap() failed" );
        return -1;
    }
    else
    {
        log_writeln_fmt( C_log::LL_VERBOSE_3, "mmap() successful, gpio_reg_ = %p", gpio_reg_ );
    }

    return 0;
}

void
C_led_morse::gpio_set_mode( unsigned gpio, unsigned mode )
{
   int reg   =  gpio / 10;
   int shift = ( gpio % 10 ) * 3;

   gpio_reg_[ reg ] = ( gpio_reg_[ reg ] & ~ ( 7 << shift ) ) | ( mode << shift );
}

void
C_led_morse::gpio_set_pullupdown( unsigned gpio, unsigned pud )
{
   * ( gpio_reg_ + GPPUD ) = pud;

   delay( 20 );

   * ( gpio_reg_ + GPPUDCLK0 + PI_BANK ) = PI_BIT;

   delay( 20 );

   * ( gpio_reg_ + GPPUD ) = 0;

   * ( gpio_reg_ + GPPUDCLK0 + PI_BANK ) = 0;
}

void
C_led_morse::gpio_write( unsigned gpio, unsigned level )
{
    if ( level == 0 )
    {
        * ( gpio_reg_ + GPCLR0 + PI_BANK ) = PI_BIT;
    }
    else
    {
        * ( gpio_reg_ + GPSET0 + PI_BANK ) = PI_BIT;
    }
}

unsigned int
C_led_morse::gpio_hardware_revision()
{
    static unsigned rev = 0;

    char  buf[512];
    char  term;
    int   revision_chars = 4;      // number of revision_chars in revision string */

    FILE *filp;

    if ( rev )
    {
        return rev;
    }

    pi_model_ = 0;

    filp = fopen ( "/proc/cpuinfo", "r" );

    if ( filp != NULL )
    {
        while ( fgets( buf, sizeof( buf ), filp ) != NULL )
        {
            if ( pi_model_ == 0 )
            {
                if ( ! strncasecmp( "model name", buf, 10 ) )
                {
                    if ( strstr( buf, "ARMv6" ) != NULL )
                    {
                        pi_model_ = 1;
                        revision_chars = 4;
                    }
                    else if ( strstr( buf, "ARMv7" ) != NULL )
                    {
                        pi_model_ = 2;
                        revision_chars = 6;
                    }
                    else if ( strstr( buf, "ARMv8" ) != NULL )
                    {
                        pi_model_ = 2;
                        revision_chars = 6;
                    }
                }
            }

            if ( ! strncasecmp( "revision", buf, 8 ) )
            {
                if ( sscanf( buf + strlen( buf ) - ( revision_chars + 1 ), "%x%c", &rev, &term ) == 2 )
                {
                    if ( term != '\n' )
                    {
                        rev = 0;
                    }
                }
            }
        }

        fclose( filp );
    }
    
    return rev;
}

}
