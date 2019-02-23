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

// main_state.h

#ifndef main_state_H
#define main_state_H

#include "single.h"


using namespace std;

namespace morsamdesa
{

class C_main_proc;


class C_main_state
{
public:

    C_main_state() {}
    virtual ~C_main_state() {}
    
    virtual void
    handler( C_main_proc * p );

protected:

    void
    change_state_to( C_main_proc * main_proc, shared_ptr< C_main_state > main_state, const char * description );

};

class C_wait_for_message : public C_main_state
{
public:

    static C_single< C_wait_for_message, C_main_state > s;

public:

    virtual void
    handler( C_main_proc * p );

private:

};

class C_preamble : public C_main_state
{
public:

    static C_single< C_preamble, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_preamble_wait : public C_main_state
{
public:

    static C_single< C_preamble_wait, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_message_send : public C_main_state
{
public:

    static C_single< C_message_send, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_message_send_wait: public C_main_state
{
public:

    static C_single< C_message_send_wait, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_message_end : public C_main_state
{
public:

    static C_single< C_message_end, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_follow_on_wait : public C_main_state
{
public:

    static C_single< C_follow_on_wait, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_inter_message : public C_main_state
{
public:

    static C_single< C_inter_message, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_inter_message_wait : public C_main_state
{
public:

    static C_single< C_inter_message_wait, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_postamble : public C_main_state
{
public:

    static C_single< C_postamble, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

class C_postamble_wait : public C_main_state
{
public:

    static C_single< C_postamble_wait, C_main_state > s;

protected:

    virtual void
    handler( C_main_proc * p );

};

}

#endif  // main_state_H
