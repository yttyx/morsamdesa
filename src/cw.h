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

// cw.h

#ifndef C_H
#define C_H

#include <memory>

#include "common.h"
#include "sample_source.h"

namespace morsamdesa
{

class C_cw : public C_sample_source
{

public:

    C_cw( const S_transmitter & transmitter );
    virtual ~C_cw();

    virtual bool
    initialise( unsigned int samples, shared_ptr< C_audio_output > output, int frequency_shift );

    virtual void
    write( bool & samples_exhausted );

private:

    bool
    init_edge_envelopes();

    void
    release_edge_envelopes();

    bool
    init_buffer( unsigned int samples, int frequency_shift );

    bool
    generate_sample_buffer( unsigned int samples, int frequency_shift );

    double
    calc_sample_level( unsigned int sample_count, double frequency );

    bool
    shape_waveform();

    void
    free_resources();

    void
    free_buffer( short int **buffer );

    void
    free_buffer( double **buffer );

private:

    unsigned int rising_edge_samples_;
    unsigned int falling_edge_samples_;

    unsigned int noise_burst_start_samples_;
    unsigned int noise_burst_end_samples_;

    double *rising_edge_;
    double *falling_edge_;

    // Levels (exponential scale)
    double h1_level_;
    double h2_level_;
    double h3_level_;
    double h4_level_;
    double noise_level_;
    double noise_burst_start_level_;
    double noise_burst_end_level_;
    bool   noise_burst_start_;
    bool   noise_burst_end_;

    S_transmitter transmitter_;
};

}

#endif  // C_H
