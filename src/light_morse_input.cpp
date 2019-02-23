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

// light_morse_input.cpp

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gpio.h"
#include "light_morse_input.h"
#include "log.h"
#include "misc.h"


using namespace std;
using namespace morsamdesa;

namespace morsamdesa
{

extern C_log  log;
extern C_gpio gpio;

C_light_morse_input::C_light_morse_input()
{
}

C_light_morse_input::~C_light_morse_input()
{
}

// -----------------------------------------------------------------------------------
// Foreground thread code
// -----------------------------------------------------------------------------------
bool
C_light_morse_input::initialise()
{
    loop_delay_   = 40;
    invert_input_ = false;
    abort_        = false;
    state_        = stAwaitOn;

    morse_timing_.reset( new C_morse_timing( 20.0, 16.0, 1.0, 1.0, MM_CW, MC_INTERNATIONAL ) );
    pulse_to_morse_.reset( new C_pulse_to_morse( morse_timing_->duration_dot() ) );

    return true;
}

bool
C_light_morse_input::start()
{
    return thread_start();
}

void
C_light_morse_input::stop()
{
    abort_ = true;

    thread_await_exit();
}

eCommand
C_light_morse_input::read()
{
    eCommand cmd = cmdNone;

    buffer_lock_.lock();

    if ( buffer_.size() > 0 )
    {
        cmd = buffer_.front();
        buffer_.pop();
    }

    buffer_lock_.unlock();

    return cmd;
}

// -----------------------------------------------------------------------------------
// Background thread code
// -----------------------------------------------------------------------------------

void
C_light_morse_input::thread_handler()
{
    while ( ! abort_ )
    {
        switch ( state_ )
        {
            case stInit:
                init();
                break;
            case stAwaitOn:
                await_on();
                break;
            case stAwaitOff:
                await_off();
                break;
            case stOffToOnTransition:
                off_to_on_transition();
                break;
            case stOnToOffTransition:
                on_to_off_transition();
                break;
            case stEndOfCharacter:
                end_of_character();
                break;
            default:
                break;
        }
    }
}

void
C_light_morse_input::init()
{
    input_inversion_timer_.start( INPUT_INVERT_TIME_MS );

    state_ = stAwaitOn;
}

void
C_light_morse_input::await_on()
{
    if ( light_on() )
    {
        state_ = stOffToOnTransition;
    }
    else
    {
        if ( state_timer_.elapsed_ms() > morse_timing_->duration_interletter() )
        {
            state_timer_.stop();
            
            state_ = stEndOfCharacter;
        }
        else
        {
            delay( loop_delay_ );
        }
    }
}

void
C_light_morse_input::off_to_on_transition()
{
    input_inversion_timer_.start( INPUT_INVERT_TIME_MS );
    state_timer_.start( 0 );

    state_ = stAwaitOff;
}

void
C_light_morse_input::await_off()
{
    if ( ! light_on() )
    {
        state_ = stOnToOffTransition;
    }
    else
    {
        // Input is still on
        if ( state_timer_.elapsed_ms() > morse_timing_->duration_interletter() )
        {
            reset();
        }
        else
        {
            delay( loop_delay_ );
        }

        if ( input_inversion_timer_.expired() )
        {
            reset();
            invert_input_ = ! invert_input_;

            state_ = stAwaitOn;
        }
    }
}

void
C_light_morse_input::on_to_off_transition()
{
    pulse_to_morse_->add( state_timer_.elapsed_ms() );

    state_timer_.stop();
    state_timer_.start( 0 );

    state_ = stAwaitOn;
}

void
C_light_morse_input::end_of_character()
{
    string morse = pulse_to_morse_->read();
    
    pulse_to_morse_->reset();

    const char * cmd = NULL;

    if ( morse == "--" )
    {
        buffer_lock_.lock();
        buffer_.push( cmdMute );
        buffer_lock_.unlock();

        cmd = "Mute";
    }
    else if ( morse == ".--." )
    {
        buffer_lock_.lock();
        buffer_.push( cmdPrevious );
        buffer_lock_.unlock();

        cmd = "Previous";
    }
    else if ( morse == ".-." )
    {
        buffer_lock_.lock();
        buffer_.push( cmdRpt );
        buffer_lock_.unlock();

        cmd = "Repeat";
    }
    else if ( morse == "-." )
    {
        buffer_lock_.lock();
        buffer_.push( cmdNext );
        buffer_lock_.unlock();

        cmd = "Next";
    }
    else if ( morse == ".." )
    {
        buffer_lock_.lock();
        buffer_.push( cmdStop );
        buffer_lock_.unlock();

        cmd = "Stop";
    }
    else if ( morse == "..." )
    {
        buffer_lock_.lock();
        buffer_.push( cmdPrefix );
        buffer_lock_.unlock();

        cmd = "Toggle prefix";
    }

    if ( cmd != NULL )
    {
        log_writeln_fmt( C_log::LL_INFO, "Command: %s ", cmd );
    }

    state_ = stAwaitOn;
}

void
C_light_morse_input::reset()
{
    state_timer_.stop();
    pulse_to_morse_->reset();
}

bool
C_light_morse_input::light_on()
{
    bool light_detected = gpio.read( GPIO_SENSOR );

    return invert_input_ ? ( ! light_detected ) : light_detected;
}

const char *
C_light_morse_input::to_string( eState state )
{
    switch ( state_ )
    {
        case stInit:
            return "stInit";
        case stAwaitOn:
            return "stAwaitOn";
        case stAwaitOff:
            return "stAwaitOff";
        case stOffToOnTransition:
            return "stOffToOnTransition";
        case stOnToOffTransition:
            return "stOnToOffTransition";
        case stEndOfCharacter:
            return "stEndOfCharacter";
        default:
            return "?";
    }

    return "?";
}

}
