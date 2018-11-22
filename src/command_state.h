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

// command_state.h

#ifndef command_state_H
#define command_state_H

#include "single.h"


namespace morsamdesa
{

class C_main_proc;


class C_command_state
{
public:

    C_command_state() {}
    virtual ~C_command_state() {}

    virtual void
    handler( C_main_proc * p );

protected:

    void
    change_state_to( C_main_proc * main_proc, C_command_state * state, const char * description );

};

class C_wait_for_command : public C_command_state
{
public:

    static C_single< C_wait_for_command, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_check_night_mode : public C_command_state
{
public:

    static C_single< C_check_night_mode, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_mute: public C_command_state
{
public:

    static C_single< C_mute, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_unmute: public C_command_state
{
public:

    static C_single< C_unmute, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_toggle_mute: public C_command_state
{
public:

    static C_single< C_toggle_mute, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_mute_unmute_wait : public C_command_state
{
public:

    static C_single< C_mute_unmute_wait, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_previous_message: public C_command_state
{
public:

    static C_single< C_previous_message, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_previous_message_wait : public C_command_state
{
public:

    static C_single< C_previous_message_wait, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_next_message: public C_command_state
{
public:

    static C_single< C_next_message, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_next_message_wait : public C_command_state
{
public:

    static C_single< C_next_message_wait, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_interrupt_message: public C_command_state
{
public:

    static C_single< C_interrupt_message, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_interrupt_message_wait : public C_command_state
{
public:

    static C_single< C_interrupt_message_wait, C_command_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

}

#endif  // command_state_H
