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

// morsamdesa.h

#ifndef morsamdesa_H
#define morsamdesa_H

#include "main_proc.h"
#include "remote_control.h"


using namespace std;

namespace morsamdesa
{

class C_morsamdesa
{
public:
    C_morsamdesa();
    ~C_morsamdesa();

    void
    run( int argc, char *argv[] );

private:

    bool
    abort();

    void
    do_datafeeds();

private:

    C_main_proc         main_;
    C_remote_stub       *rc_;
};

}

#endif // morsamdesa_H
