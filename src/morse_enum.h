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

// morse_enum.h

#ifndef morse_enum_H
#define morse_enum_H

namespace morsamdesa
{

enum eMorseCode
{
    MC_INTERNATIONAL,
    MC_AMERICAN,
    MC_INVALID
};

enum eMorseMode
{
    MM_CW,
    MM_DOUBLE_PLATE_SOUNDER,
    MM_LED,
    MM_SOUNDER,
    MM_SPARKGAP,
    MM_INVALID
};

};

#endif // morse_enum_H
