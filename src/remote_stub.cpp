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

// remote_stub.cpp

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "remote_stub.h"


using namespace std;
using namespace morsamdesa;

namespace morsamdesa
{

extern C_log log;

bool
C_remote_stub::initialise()
{
    log_writeln( C_log::LL_INFO, "Remote control disabled" );

    return true;
}

}
