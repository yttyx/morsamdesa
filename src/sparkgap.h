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

// sparkgap.h

#ifndef C_sparkgap_H
#define C_sparkgap_H

#include "sound_file.h"


namespace morsamdesa
{

class C_sparkgap : public C_sound_file
{

public:

    C_sparkgap( const char *description, const string & filename, unsigned int samples, float level );
    ~C_sparkgap();

    bool
    initialise( shared_ptr< C_audio_output > output );

};

}

#endif // C_sparkgap_H
