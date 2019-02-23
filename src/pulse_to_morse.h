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

// pulse_to_morse.h

#ifndef pulse_to_morse_H
#define pulse_to_morse_H

using namespace std;


namespace morsamdesa
{

class C_pulse_to_morse
{

public:

    static const unsigned int DURATIONS_MAX = 10;

    C_pulse_to_morse( unsigned int dot_duration );
    ~C_pulse_to_morse(){}

    void
    add( unsigned int duration );

    void
    reset();

    string
    read();

private:

    C_pulse_to_morse(){}

    unsigned int durations_[ DURATIONS_MAX + 1 ];

    unsigned int duration_curr_;
    unsigned int duration_dot_;

    string morse_;
};

}

#endif // pulse_to_morse_H
