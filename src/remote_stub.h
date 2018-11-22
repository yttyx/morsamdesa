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

// remote_stub.h

#ifndef    remote_stub_I
#define    remote_stub_I

#include "command.h"


namespace morsamdesa
{

class C_remote_stub
{

public:

    C_remote_stub() { abort_ = false; }
    virtual ~C_remote_stub() {}

    virtual bool
    initialise();

    virtual bool
    start() { return true; }

    virtual void
    stop() {}

    virtual eCommand
    read() { return cmdNone; }

protected:

    bool    abort_;

};

}

#endif    // remote_stub_I
