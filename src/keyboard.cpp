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

// C_keyboard.cpp

#include <unistd.h>

#include "keyboard.h"


namespace morsamdesa
{

C_keyboard kbd;

C_keyboard::C_keyboard()
{
    tcgetattr( 0, &termios_original );

    termios_current = termios_original;

    termios_current.c_lflag &= ~ICANON;     // select non-canonical mode (don't wait for line terminator)
    termios_current.c_lflag &= ~ECHO;       // don't echo input characters
    termios_current.c_lflag &= ~ISIG;       // if INTR, QUIT, SUSP, or DSUSP are received, don't generate the corresponding signal
                                            // (pass through the Ctrl-C, Ctrl-Z etc.)

    termios_current.c_cc[ VMIN ]  = 1;
    termios_current.c_cc[ VTIME ] = 0;

    tcsetattr( 0, TCSANOW, &termios_current );

    kbd_ch_ = NO_KEY;
}

C_keyboard::~C_keyboard()
{
    // Restore original setup
    tcsetattr( 0, TCSANOW, &termios_original );
}

bool
C_keyboard::got_keypress()
{
    if ( kbd_ch_ == NO_KEY )
    {
        termios_current.c_cc[ VMIN ] = 0;

        tcsetattr( 0, TCSANOW, &termios_current );

        unsigned char ch;
        int           nread;

        nread = read( 0, &ch, 1 );

        termios_current.c_cc[ VMIN ] = 1;

        tcsetattr( 0, TCSANOW, &termios_current );

        if ( nread == 1 )
        {
            kbd_ch_ = ch;
            return true;
        }
    }
    else
    {
        return true;
    }

    return false;
}

int
C_keyboard::get_key()
{
    char ch;

    if ( kbd_ch_ != NO_KEY )
    {
        ch = kbd_ch_;
        kbd_ch_ = NO_KEY;
    }
    else
    {
        read( 0, &ch, 1 );
    }

    return ch;
}

}
