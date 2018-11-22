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

// single.h

#ifndef single_H
#define single_H

namespace morsamdesa
{

template< class derived, class base >
class C_single
{
public:

    C_single();
    ~C_single();

    static base *instance()
    {
        if ( instance_ == NULL )
        {
            instance_ = new derived();
        }

        return instance_;
    }

protected:

    C_single( const C_single & ) {}

public:

    static base *instance_;

};

template< class derived, class base >
base * C_single< derived, base >::instance_ = NULL;

}

#endif // single_H
