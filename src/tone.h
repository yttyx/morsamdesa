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

// tone.h

#ifndef C_tone_H
#define C_tone_H

#include "common.h"
#include "sample_source.h"
#include "tone_type.h"

namespace morsamdesa
{

enum eInterval
{
    eDown8ve    = -12,
    eDownMaj7th = -11,
    eDownMin7th = -10,
    eDownMaj6th = -9,
    eDownMin6th = -8,
    eDown5th    = -7,
    eDownDim5th = -6,
    eDown4th    = -5,
    eDownMaj3rd = -4,
    eDownMin3rd = -3,
    eDownMaj2nd = -2,
    eDownMin2nd = -1,
    eNoChange   =  0,
    eUpMin2nd   =  1,
    eUpMaj2nd   =  2,
    eUpMin3rd   =  3,
    eUpMaj3rd   =  4,
    eUp4th      =  5,
    eUpDim5th   =  6,
    eUp5th      =  7,
    eUpMin6th   =  8,
    eUpMaj6th   =  9,
    eUpMin7th   = 10,
    eUpMaj7th   = 11,
    eUp8ve      = 12,
    eInvalid    = 666
};

const int max_intervals = 5;

struct S_tone_def
{
    const char * morse;
    eInterval    intervals[ max_intervals ];
};


class C_tone : public C_sample_source
{

public:

    C_tone();
    virtual ~C_tone();

    virtual bool
    initialise( eToneType tone_type, C_audio_output * output );

    virtual void
    write( bool & samples_exhausted );

private:

    bool
    init_envelope_buffers();

    bool
    init_edge_envelopes();

    void
    release_edge_envelopes();

    bool
    init_tone_buffer( eToneType tone_type );

    void
    get_tone_settings( eToneType tone_type, S_tone_def *& tone_def, bool & padding, bool & harmonics );

    bool
    generate_sample_buffer( S_tone_def & tones, bool harmonics, bool padding );

    double
    calc_sample_level( unsigned int sample_count, double frequency, bool harmonics );

    void
    complete_cw_tone();

    void
    free_resources();

    void
    free_buffer( short int **buffer );

    void
    free_buffer( double **buffer );

    void
    dump_buffer( const char * desc, short int * buffer, unsigned int samples );

private:

    double *cw_rising_edge_;
    double *cw_falling_edge_;

    // Levels (exponential scale)
    double cw_h1_level_;
    double cw_h2_level_;
    double cw_h3_level_;
    double cw_h4_level_;
    double cw_noise_level_;
    double cw_noise_burst_start_level_;
    double cw_noise_burst_end_level_;
    bool   cw_noise_burst_start_;
    bool   cw_noise_burst_end_;
};

}

#endif  // C_tone_H
