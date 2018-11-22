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

// keyboard.h

#ifndef    keyboard_I
#define    keyboard_I

#include <termios.h>

namespace morsamdesa
{

class C_keyboard
{

public:


    C_keyboard();
    ~C_keyboard();

    bool
    got_keypress();

    int
    get_key();

    static const int ABORT_KEY_1    = 0x03;   // Ctrl-C
    static const int ABORT_KEY_2    = 0x1b;   // <Esc>
    static const int NO_KEY         = -1;

private:

    int kbd_ch_;

private:

    struct termios termios_original;
    struct termios termios_current;

};

}

#endif    // keyboard_I
