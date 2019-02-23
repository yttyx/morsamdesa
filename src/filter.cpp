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

// filter.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "filter.h"
#include "log.h"
#include "misc.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;


C_filter::C_filter()
{
}

C_filter::~C_filter()
{
    release_resources();
}

bool
C_filter::add_filters( const vector< string > & filter_strings )
{
    bool worked = true;

    for ( unsigned int ii = 0; ii < filter_strings.size(); ii++ )
    {
        regex_t reg;

        int regcomp_res = regcomp( &reg, filter_strings[ ii ].c_str(), REG_NOSUB | REG_EXTENDED | REG_ICASE );

        if ( regcomp_res == 0 )
        {
            filters_.push_back( reg );
        }
        else
        {
            log_writeln_fmt( C_log::LL_ERROR, "Regex compilation error: %s", regex_error( regcomp_res, reg ).c_str() );
            worked = false;
        }
    }

    return worked;
}

bool
C_filter::is_acceptable( const char *str )
{
    assert( str );

    for ( unsigned int ii = 0; ii < filters_.size(); ii++ )
    {
        if ( regexec( &filters_[ ii ], str, 0, NULL, 0 ) == 0 )
        {
            return false;
        }
    }

    return true;
}

void
C_filter::release_resources()
{
    for ( unsigned int ii = 0; ii < filters_.size(); ii++ )
    {
        regfree( &filters_[ ii ] );
    }
}

}
