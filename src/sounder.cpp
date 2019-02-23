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

// sounder.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "sounder.h"


using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;


C_sounder::C_sounder( const char *description, const string & filename, unsigned int samples, float level )
    : C_sound_file( description, filename, smOneShot, samples, level )
{
}

C_sounder::~C_sounder()
{
}

bool
C_sounder::initialise( shared_ptr< C_audio_output > output )
{
    output_ = output;

    return read();       // Read in sample file
}

}
