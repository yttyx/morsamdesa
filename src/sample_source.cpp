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

// sample_source.cpp

#include "config.h"
#include "log.h"
#include "misc.h"
#include "sample_source.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;

C_sample_source::C_sample_source()
{
    active_      = false;
    samples_     = 0;
    sample_curr_ = 0;
    buffer_      = NULL;
    output_      = NULL;
}

C_sample_source::~C_sample_source()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_sample_source destructor" );

    if ( buffer_ )
    {
        delete buffer_;
    }
}

}
