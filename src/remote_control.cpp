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

// remote_control.cpp

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "remote_control.h"


using namespace std;
using namespace morsamdesa;

namespace morsamdesa
{

extern C_log log;

C_remote_control remote;

C_remote_control::C_remote_control()
{
    handle_ = -1;

    memset( &pollfd_, 0, sizeof( pollfd_ ) );
}

C_remote_control::~C_remote_control()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_remote_control destructor" );

    if ( handle_ >= 0 )
    {
        close( handle_ );
    }
}

// -----------------------------------------------------------------------------------
// Foreground thread code
// -----------------------------------------------------------------------------------
bool
C_remote_control::initialise()
{
    device_ = "/dev/hidraw1";

    handle_ = ::open( device_.c_str(), O_RDONLY | O_NONBLOCK );

    if ( handle_ < 0 )
    {
        log_writeln_fmt( C_log::LL_ERROR, "**Error opening device %s: %s", device_.c_str(), strerror( errno ) );
        return false;
    }
    else
    {
        pollfd_.fd     = handle_;
        pollfd_.events = POLLIN;

        log_writeln( C_log::LL_INFO, "Remote control enabled" );
    }

    return true;
}

// -----------------------------------------------------------------------------------
// Foreground thread code
// -----------------------------------------------------------------------------------
bool
C_remote_control::start()
{
    return thread_start();
}

void
C_remote_control::stop()
{
    abort_ = true;

    thread_await_exit();
}

eCommand
C_remote_control::read()
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

const eCommand C_remote_control::remote_table[] =
{
//                  ----------- FLIRC -----------------
//                  Code (hex)  Key legend      Command
//                  ----------  -------------   ------------------------------------------------
    cmdNone,        //  00
    cmdNone,        //  01
    cmdNone,        //  02
    cmdNone,        //  03
    cmdNext,        //  04      Next            Play next message/move to next sequence in timer
    cmdNone,        //  05
    cmdTimer9,      //  06      9               Select timer 9
    cmdTimer1,      //  07      1               Select timer 1
    cmdVolUp,       //  08      Vol+            Increase volume
    cmdTimer2,      //  09      2               Select timer 2
    cmdTimer3,      //  0a      3               Select timer 3
    cmdTimer4,      //  0b      4               Select timer 4
    cmdEnter,       //  0c      Enter/Save      Not used
    cmdTimer5,      //  0d      5               Select timer 5
    cmdTimer6,      //  0e      6               Select timer 6
    cmdNone,        //  0f
    cmdNone,        //  10
    cmdNone,        //  11
    cmdNone,        //  12
    cmd10plus,      //  13      10+
    cmdVolDown,     //  14      Vol-            Decrease volume
    cmdSetup,       //  15      Setup
    cmdRpt,         //  16      Rpt
    cmdPrevious,    //  17      Prev            Play previous message
    cmdChanDown,    //  18      Ch-
    cmdNone,        //  19
    cmdMute,        //  1a      >||             Mute/Unmute
    cmdNone,        //  1b
    cmdStop,        //  1c      Stop            Interrupt current message/timer
    cmdTimer7       //  1d      7               Select timer 7
};


void
C_remote_control::thread_handler()
{
    while ( ! abort_ )
    {
        // Check for received data every 200 mS
        if ( poll( &pollfd_, 1, 200 ) < 0 )
        {
            log_writeln_fmt( C_log::LL_ERROR, "Error polling device %s: %s", device_.c_str(), strerror( errno ) );
            abort_ = true;
            break;
        }

        short int revents = pollfd_.revents;

        if ( revents & POLLIN )
        {
            char buf[ 8 ];
            
            int res = ::read( handle_, buf, 3 );

            if ( res < 0 )
            {
                log_writeln_fmt( C_log::LL_ERROR, "Error reading from device %s: %s", device_.c_str(), strerror( errno ) );
            }
            else
            {
                log_writeln_fmt( C_log::LL_VERBOSE_3, "Read: %02x %02x %02x", buf[ 0 ], buf[ 1 ], buf[ 2 ] );

                if ( res == 3 )
                {
                    eCommand cmd = cmdNone;

                    if ( buf[ 2 ] <= 0x1d )
                    {
                        cmd = remote_table[ ( int ) buf[ 2 ] ];
                    }

                    buffer_lock_.lock();
                    buffer_.push( cmd );
                    buffer_lock_.unlock();
                }
            }
        }
    }
}

}
