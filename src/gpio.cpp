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

// gpio.cpp
//

#include <assert.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "gpio.h"
#include "log.h"
#include "misc.h"

using namespace morsamdesa;
using namespace std;


namespace morsamdesa
{

extern C_log log;

C_gpio gpio;


C_gpio::C_gpio()
{
    gpio_reg_    = ( unsigned int * ) MAP_FAILED;
    pi_model_    = 0;
    pi_revision_ = 0;
}

bool
C_gpio::initialise()
{
    gpio_lock_.lock();

    // Set pi_model and pi_revision_
    pi_revision_ = gpio_hardware_revision();

    int fd = open( "/dev/gpiomem", O_RDWR | O_SYNC );

    if ( fd < 0 )
    {
        log_writeln( C_log::LL_ERROR, "C_gpio::initialise(): Failed to open /dev/gpiomem" );
        gpio_lock_.unlock();
        return false;
    }

    gpio_reg_ = ( unsigned int * ) mmap( NULL, 0xB4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

    close( fd );

    if ( gpio_reg_ == MAP_FAILED )
    {
        log_writeln( C_log::LL_ERROR, "C_gpio::initialise(): mmap() failed" );
        gpio_lock_.unlock();
        return false;
    }
    else
    {
        log_writeln_fmt( C_log::LL_VERBOSE_3, "mmap() successful, gpio_reg_ = %p", gpio_reg_ );
    }

    gpio_set_pullupdown( 4, PI_PUD_UP );        // LED output
    gpio_set_mode( 4, PI_OUTPUT );              //

    gpio_set_pullupdown( 17, PI_PUD_OFF );      // Light sensor input
    gpio_set_mode( 17, PI_INPUT );              //

    gpio_lock_.unlock();
 
    return true;
}

C_gpio::~C_gpio()
{
}

void
C_gpio::write( unsigned gpio, unsigned level )
{
    gpio_lock_.lock();

    if ( level == 0 )
    {
        * ( gpio_reg_ + GPCLR0 + PI_BANK ) = PI_BIT;
    }
    else
    {
        * ( gpio_reg_ + GPSET0 + PI_BANK ) = PI_BIT;
    }

    gpio_lock_.unlock();
}

bool
C_gpio::read( unsigned gpio )
{
    gpio_lock_.lock();

    bool result = ( ( * ( gpio_reg_ + GPLEV0 + PI_BANK ) & PI_BIT ) != 0 ) ? true : false;

    gpio_lock_.unlock();

    return result;
}

void
C_gpio::gpio_set_mode( unsigned gpio, unsigned mode )
{
    int reg   =  gpio / 10;
    int shift = ( gpio % 10 ) * 3;

    gpio_reg_[ reg ] = ( gpio_reg_[ reg ] & ~ ( 7 << shift ) ) | ( mode << shift );
}

void
C_gpio::gpio_set_pullupdown( unsigned gpio, unsigned pud )
{
    * ( gpio_reg_ + GPPUD ) = pud;

    delay( 20 );

    * ( gpio_reg_ + GPPUDCLK0 + PI_BANK ) = PI_BIT;

    delay( 20 );

    * ( gpio_reg_ + GPPUD ) = 0;

    * ( gpio_reg_ + GPPUDCLK0 + PI_BANK ) = 0;
}

unsigned int
C_gpio::gpio_hardware_revision()
{
    static unsigned rev = 0;

    char  buf[512];
    char  term;
    int   revision_chars = 4;      // Number of revision_chars in revision string

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
