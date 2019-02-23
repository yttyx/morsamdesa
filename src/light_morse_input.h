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

// light_morse_input.h

#ifndef    light_morse_input_I
#define    light_morse_input_I

#include <memory>
#include <queue>
#include <string>


#include "command.h"
#include "morse_timing.h"
#include "pulse_to_morse.h"
#include "thread.h"
#include "timer.h"

using namespace std;

namespace morsamdesa
{

class C_light_morse_input : public C_thread
{
    enum eState
    {
        stInvalid
    ,   stInit
    ,   stAwaitOn
    ,   stAwaitOff
    ,   stEndOfCharacter
    ,   stOffToOnTransition
    ,   stOnToOffTransition
    };

    const unsigned int INPUT_INVERT_TIME_MS = 1500;

public:

    C_light_morse_input();
    ~C_light_morse_input();

    bool
    initialise();

    bool
    start();

    void
    stop();

    eCommand
    read();

private:

    void
    init();

    void
    await_on();

    void
    await_off();

    void
    off_to_on_transition();

    void
    on_to_off_transition();

    void
    end_of_character();

    void
    thread_handler();

    bool
    light_on();

    void
    reset();

    const char *
    to_string( eState state );

private:

    eState              state_;

    bool                abort_;
    bool                invert_input_;
    unsigned int        loop_delay_;
                        
    unsigned int        duration_dot_min_;
    unsigned int        duration_dot_max_;
                        
    unsigned int        duration_dash_min_;
    unsigned int        duration_dash_max_;
                        
    string              input_;

    unique_ptr< C_morse_timing >   morse_timing_;
    unique_ptr< C_pulse_to_morse > pulse_to_morse_;

    queue< eCommand >   buffer_;

    C_timer             state_timer_;
    C_timer             input_inversion_timer_;
    C_mutex             buffer_lock_;
};

}

#endif    // light_morse_input_I
