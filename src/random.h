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

// random.h

#ifndef RANDOM_H
#define RANDOM_H

namespace morsamdesa
{

class C_random
{
public:
    C_random( int max );
    ~C_random();

    static void
    init();

    unsigned int
    next();

    unsigned int
    next_unique();

    void
    remove( int val );

    void
    display_available();

private:
    C_random() {}
    C_random( const C_random & ) {}

    void
    init_values();

    bool
    spare_slot();

    unsigned int
    next_internal( int unique );

private:
    int  max_;
    bool *values_;
    
};

}

#endif // RANDOM_H
