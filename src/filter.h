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

// filter.h

#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <vector>
#include <regex.h>

using namespace std;

namespace morsamdesa
{

class C_filter
{
public:

    C_filter();
    ~C_filter();

    bool
    add_filters( const vector< string > & filter_strings );

    bool
    is_acceptable( const char *str );

protected:

    C_filter( const C_filter & ) {}

private:

    void
    release_resources();

private:

    vector< regex_t >  filters_;

};

}

#endif // FILTER_H
