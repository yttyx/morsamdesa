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

// silence.h

#ifndef C_silence_H
#define C_silence_H

#include <memory>

#include "sample_source.h"


namespace morsamdesa
{

class C_silence : public C_sample_source
{

public:

    C_silence();
    virtual ~C_silence();

    virtual void
    write();

    virtual void
    write( bool & samples_exhausted );

    virtual bool
    initialise( unsigned int samples, shared_ptr< C_audio_output > output );

};

}

#endif // C_silence_H
