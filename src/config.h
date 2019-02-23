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

// config.h

#ifndef config_H
#define config_H

#include <regex.h>

#include <string>
#include <vector>

#include <libconfig.h++>

#include "morse_enum.h"


using namespace std;
using namespace libconfig;

namespace morsamdesa
{

enum eOption
{
    OPT_NONE,
    OPT_DISPLAY_VERBOSITY,
    OPT_DISPLAY_DATETIME,
    OPT_OUTPUT_MODE,
    OPT_OUTPUT_FILE,
    OPT_OUTPUT_LED,
    OPT_DELAY_FOLLOW_ON,
    OPT_DELAY_INTERMESSAGE,
    OPT_NIGHTMODE_START,
    OPT_NIGHTMODE_END,
    OPT_REMOTE_ENABLED,
    OPT_FEED_HEADLINES_ENABLED,
    OPT_FEED_HEADLINES_PREFIX,
    OPT_FEED_HEADLINES_URLS,
    OPT_FEED_HEADLINES_DELAY_MIN,
    OPT_FEED_HEADLINES_DELAY_MAX,
    OPT_FEED_HEADLINES_FILTERS,
    OPT_FEED_TIME_ENABLED,
    OPT_FEED_TIME_INTERVAL,
    OPT_FEED_FIXED_ENABLED,
    OPT_FEED_FIXED_MESSAGE,
    OPT_FEED_WORDLIST_ENABLED,
    OPT_FEED_WORDLIST_REPEAT,
    OPT_FEED_WORDLIST_RANDOM,
    OPT_FEED_WORDLIST_FILE,
    OPT_FEED_QUEUE_LENGTH,
    OPT_FEED_QUEUE_UNHELD,
    OPT_FEED_QUEUE_DISCARDABLE,
    OPT_NOISE_BG_FILE,
    OPT_NOISE_BG_FADE_TIME,
    OPT_NOISE_BG_LEVEL_QUIESCENT,
    OPT_NOISE_BG_LEVEL_ACTIVE,
    OPT_TRANSMITTERS,
    // Headline feed URL options
    OPT_URL_MNEMONIC,
    OPT_URL,
    // Transmitter options
    OPT_TRANSMITTER_MNEMONIC,
    OPT_CODE,
    OPT_MODE,
    OPT_ALPHANUMERIC_ONLY,
    OPT_LEVEL,
    OPT_INTER_MESSAGE_TIME,
    OPT_SPEED_AUDIO_CHAR,
    OPT_SPEED_AUDIO_EFFECTIVE,
    OPT_SPEED_LED_CHAR,
    OPT_SPEED_LED_EFFECTIVE,
    OPT_INTERVAL_MULT_CHAR,
    OPT_INTERVAL_MULT_CHAR_E,
    OPT_INTERVAL_MULT_WORD,
    OPT_INTERVAL_MULT_WORD_E,
    OPT_SOUNDER_ARM_DOWN,
    OPT_SOUNDER_ARM_UP,
    OPT_SOUNDER_LO,
    OPT_SOUNDER_HI,
    OPT_SPARKGAP_DOT,
    OPT_SPARKGAP_DASH,
    OPT_CW_H1_FREQUENCY,
    OPT_CW_H1_LEVEL,
    OPT_CW_H2_MULT,
    OPT_CW_H2_LEVEL,
    OPT_CW_H3_MULT,
    OPT_CW_H3_LEVEL,
    OPT_CW_H4_MULT,
    OPT_CW_H4_LEVEL,
    OPT_CW_RISE_TIME,
    OPT_CW_FALL_TIME,
    OPT_CW_NOISE_BURST_START_DURATION,
    OPT_CW_NOISE_BURST_START_LEVEL,
    OPT_CW_NOISE_BURST_END_DURATION,
    OPT_CW_NOISE_BURST_END_LEVEL
};

enum eOutputMode
{
    OMODE_PULSEAUDIO,
    OMODE_MP3,
    OMODE_INVALID
};

enum eFieldType
{
    FLD_NONE,
    FLD_STRING,
    FLD_INTEGER,
    FLD_FLOAT,
    FLD_BOOLEAN,
    FLD_STRING_ARRAY,
    FLD_URL_ARRAY,
    FLD_TRANSMITTER_ARRAY,
    FLD_MORSE_MODE,
    FLD_MORSE_CODE,
    FLD_OUTPUT_MODE,
    FLD_LEVEL
};

struct S_option
{
    eOption       opt;
    eFieldType    field_type;
    int           field_offset;
    const char    *field_path;
    const char    *min;
    const char    *max;
};

struct S_url
{
    string mnemonic;
    string url;
};

struct S_transmitter
{
    S_transmitter()
    {
        morse_code = MC_INVALID;
        morse_mode = MM_INVALID;
        alphanumeric_only = false;
        level = 0.0;
        speed_char = 0.0;
        speed_effective = 0.0;
        interval_multiplier_char = 0.0;
        interval_multiplier_word = 0.0;
        cw_h1_frequency = 0;
        cw_h1_level = 0.0;
        cw_h2_multiplier = 0.0;
        cw_h2_level = 0.0;
        cw_h3_multiplier = 0.0;
        cw_h3_level = 0.0;
        cw_h4_multiplier = 0.0;
        cw_h4_level = 0.0;
        cw_rise_time = 0.0;
        cw_fall_time = 0.0;
        cw_noise_burst_start_duration = 0.0;
        cw_noise_burst_start_level = 0.0;
        cw_noise_burst_end_duration = 0.0;
        cw_noise_burst_end_level = 0.0;
    }

    string      mnemonic;
    eMorseCode  morse_code;
    eMorseMode  morse_mode;
    bool        alphanumeric_only;
    float       speed_char;
    float       speed_effective;
    float       interval_multiplier_char;
    float       interval_multiplier_word;
    string      sounder_arm_down;
    string      sounder_arm_up;
    string      sounder_hi;
    string      sounder_lo;
    string      sparkgap_dot;
    string      sparkgap_dash;
    int         cw_h1_frequency;
    float       level;
    float       cw_h1_level;
    float       cw_h2_multiplier;
    float       cw_h2_level;
    float       cw_h3_multiplier;
    float       cw_h3_level;
    float       cw_h4_multiplier;
    float       cw_h4_level;
    float       cw_rise_time;
    float       cw_fall_time;
    float       cw_noise_burst_start_duration;
    float       cw_noise_burst_start_level;
    float       cw_noise_burst_end_duration;
    float       cw_noise_burst_end_level;
};

struct S_config
{
    int                     display_verbosity;
    bool                    display_datetime;
    eOutputMode             output_mode;
    string                  output_file;
    bool                    output_led;
    float                   delay_follow_on;
    float                   delay_intermessage;
    string                  nightmode_start;
    string                  nightmode_end;
    bool                    feed_headlines_enabled;
    bool                    feed_headlines_prefix;
    vector< S_url >         feed_headlines_urls;
    int                     feed_headlines_delay_min;
    int                     feed_headlines_delay_max;
    vector< string >        feed_headlines_filters;
    bool                    feed_time_enabled;
    int                     feed_time_interval;
    bool                    feed_fixed_enabled;
    string                  feed_fixed_message;
    bool                    feed_wordlist_enabled;
    bool                    feed_wordlist_random;
    bool                    feed_wordlist_repeat;
    string                  feed_wordlist_file;
    int                     feed_queue_unheld;
    string                  noise_bg_file;
    float                   noise_bg_fade_time;
    float                   noise_bg_level_quiescent;
    float                   noise_bg_level_active;
    vector< S_transmitter > transmitters;
    S_transmitter           led;
    S_transmitter           command;
    S_transmitter           time;
};

struct S_derived                                        // Settings derived from config settings
{
    unsigned int tick_samples;                          // Number of samples in a 20mS tick
    unsigned int duration_tick;                         // mS
    unsigned int duration_follow_on;                    // mS
    unsigned int duration_intermessage;                 // mS
    unsigned int intermessage_samples;                  // Number of samples between messages
    unsigned int bg_noise_fade_samples;                 // Number of samples in background noise fade up/fade down
                                                        
    float bg_noise_level_active;                        //
    float bg_noise_level_quiescent;                     //
    bool  bg_noise;                                     //
    bool  bg_noise_fading;                              //
};

const char *
to_string( eOutputMode output_mode_e );

const char *
to_string( eMorseCode morse_code_e );

const char *
to_string( eMorseMode morse_mode_e );


class C_config
{
public:
    C_config();
    ~C_config();

    bool
    read( int argc, char *argv[] );

    const S_config &
    c() { return config_; }

    const S_derived &
    d() { return derived_; }

    void
    to_enum( const string & output_mode_s, eOutputMode & output_mode_e );


    void
    to_enum( const string & morse_code_s, eMorseCode & morse_code_e );


    void
    to_enum( const string & morse_mode_s, eMorseMode & morse_mode_e );

protected:

    C_config( const C_config & ){}

    bool
    check_params( int argc, char *argv[] );

    void
    usage();

    bool
    read( string & filename, string & profile_list );

    bool
    calculate_derived_settings();

    void
    display_settings();

    bool
    read_profile( const string & profile );

    void
    read_option_table( const Setting & parent, const S_option * options, void * record );

    void
    url_array( const Setting & parent, const char * field_name, vector< S_url > & url_array );

    void
    transmitter_array( const Setting & parent, const char * field_name, vector< S_transmitter > & transmitter_array );

    bool
    setting_checks();

    bool
    setting_checks( const S_option * options, void * record );

    bool
    setting_checks( const S_option * options, vector< S_url > & urls );

    bool
    setting_checks( const S_option * options, vector< S_transmitter > & transmitters );

    void
    separate_path_and_list( const string & full_path, string & path, string & list );

    const char *
    to_string( bool setting );

    const char *
    to_string( eOption opt );

    string
    to_string( eOption opt1, eOption opt2 );

    bool
    valid_time( string & time );

private:

    Config      cfg_;                   // libconfig instance
    S_config    config_;
    S_derived   derived_;

    bool        got_config_file_;
    bool        got_profile_list_;
    bool        display_config_;

    string      config_file_;
    string      profile_list_;

};

}

#endif // config_H
