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

// sound_file.h

#ifndef C_sound_file_H
#define C_sound_file_H

#include "sample_source.h"


namespace morsamdesa
{

enum eSoundMode
{
    smOneShot,
    smLoop
};

class C_sound_file : public C_sample_source
{

public:

    C_sound_file( const string & filename, eSoundMode mode );
    virtual ~C_sound_file();

    virtual void
    write( bool & samples_exhausted );

protected:

    virtual bool
    read();

private:

    eSoundMode   mode_;
    string       filename_;

};

}

#endif // C_sound_file_H
