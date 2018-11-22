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

// config.h

#ifndef config_H
#define config_H

#include <regex.h>

#include <string>
#include <vector>

#include <libconfig.h++>

using namespace std;
using namespace libconfig;

namespace morsamdesa
{

enum eOption
{
    OPT_INVALID,
    OPT_DISPLAY_VERBOSITY,
    OPT_DISPLAY_DATETIME,
    OPT_OUTPUT_MODE,
    OPT_OUTPUT_FILE,
    OPT_REMOTE_ENABLED,
    OPT_FEED_HEADLINES_ENABLED,
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
    OPT_MORSE_MODE,
    OPT_MORSE_LED_ENABLED,
    OPT_MORSE_NIGHTMODE_START,
    OPT_MORSE_NIGHTMODE_END,
    OPT_MORSE_ALPHANUMERIC_ONLY,
    OPT_MORSE_DELAY_FOLLOW_ON,
    OPT_MORSE_DELAY_INTERMESSAGE,
    OPT_MORSE_INTER_MESSAGE_TIME,
    OPT_MORSE_SPEED_AUDIO_CHAR,
    OPT_MORSE_SPEED_AUDIO_EFFECTIVE,
    OPT_MORSE_SPEED_LED_CHAR,
    OPT_MORSE_SPEED_LED_EFFECTIVE,
    OPT_MORSE_INTERVAL_MULT_CHAR,
    OPT_MORSE_INTERVAL_MULT_CHAR_E,
    OPT_MORSE_INTERVAL_MULT_WORD,
    OPT_MORSE_INTERVAL_MULT_WORD_E,
    OPT_MORSE_SOUNDER_ARM_DOWN,
    OPT_MORSE_SOUNDER_ARM_UP,
    OPT_MORSE_CW_H1_FREQUENCY,
    OPT_MORSE_CW_H1_LEVEL,
    OPT_MORSE_CW_H2_MULT,
    OPT_MORSE_CW_H2_LEVEL,
    OPT_MORSE_CW_H3_MULT,
    OPT_MORSE_CW_H3_LEVEL,
    OPT_MORSE_CW_H4_MULT,
    OPT_MORSE_CW_H4_LEVEL,
    OPT_MORSE_CW_RISE_TIME,
    OPT_MORSE_CW_FALL_TIME,
    OPT_MORSE_NOISE_BACKGROUND_FILE,
    OPT_MORSE_NOISE_BG_FILE,
    OPT_MORSE_NOISE_BG_FADE_TIME,
    OPT_MORSE_NOISE_BG_LEVEL_QUIESCENT,
    OPT_MORSE_NOISE_BG_LEVEL_ACTIVE,
    OPT_MORSE_NOISE_BG_LEVEL_DUCKING,
    OPT_MORSE_NOISE_CW_BURST_START_DURATION,
    OPT_MORSE_NOISE_CW_BURST_START_LEVEL,
    OPT_MORSE_NOISE_CW_BURST_END_DURATION,
    OPT_MORSE_NOISE_CW_BURST_END_LEVEL
};

enum eOutputMode
{
    OMODE_PULSEAUDIO,
    OMODE_MP3,
    OMODE_INVALID
};

enum eMorseMode
{
    MM_SOUNDER,
    MM_CW,
    MM_INVALID
};


struct S_config         // Config settings
{
    int                 display_verbosity;
    bool                display_datetime;
    string              output_mode;
    string              output_file;
    bool                remote_enabled;
    bool                feed_headlines_enabled;
    vector< string >    feed_headlines_urls;
    int                 feed_headlines_delay_min;
    int                 feed_headlines_delay_max;
    vector< string >    feed_headlines_filters;
    bool                feed_time_enabled;
    int                 feed_time_interval;
    bool                feed_fixed_enabled;
    string              feed_fixed_message;
    bool                feed_wordlist_enabled;
    bool                feed_wordlist_random;
    bool                feed_wordlist_repeat;
    string              feed_wordlist_file;
    int                 feed_queue_length;
    int                 feed_queue_unheld;
    int                 feed_queue_discardable;
    string              morse_mode;
    bool                morse_led_enabled;
    string              morse_nightmode_start;
    string              morse_nightmode_end;
    bool                morse_alphanumeric_only;
    float               morse_delay_follow_on;
    float               morse_delay_intermessage;
    float               morse_speed_audio_char;
    float               morse_speed_audio_effective;
    float               morse_speed_led_char;
    float               morse_speed_led_effective;
    float               morse_interval_multiplier_char;
    float               morse_interval_multiplier_word;
    string              morse_sounder_arm_down;
    string              morse_sounder_arm_up;
    int                 morse_cw_h1_frequency;
    float               morse_cw_h1_level;
    float               morse_cw_h2_multiplier;
    float               morse_cw_h2_level;
    float               morse_cw_h3_multiplier;
    float               morse_cw_h3_level;
    float               morse_cw_h4_multiplier;
    float               morse_cw_h4_level;
    float               morse_cw_rise_time;
    float               morse_cw_fall_time;
    string              morse_noise_bg_file;
    float               morse_noise_bg_fade_time;
    float               morse_noise_bg_level_quiescent;
    float               morse_noise_bg_level_active;
    float               morse_noise_cw_burst_start_duration;
    float               morse_noise_cw_burst_start_level;
    float               morse_noise_cw_burst_end_duration;
    float               morse_noise_cw_burst_end_level;
};

struct S_derived        // Settings derived from config settings
{
    eOutputMode  output_mode;
    eMorseMode   morse_mode;

    // Pulseaudio output durations
    unsigned int audio_duration_dot;                    // mS
    unsigned int audio_duration_dash;                   // mS
    unsigned int audio_duration_element;                // mS
    unsigned int audio_duration_interelement;           // mS
    unsigned int audio_duration_interletter;            // mS   actual (Farnsworth timing plus interval multiplication)
    unsigned int audio_duration_interletter_fw;         // mS   Farnsworth timing
    unsigned int audio_duration_interword;              // mS   actual (Farnsworth timing plus interval multiplication)
    unsigned int audio_duration_interword_fw;           // mS   Farnsworth timing
    unsigned int audio_duration_1min_check;             // mS

    // LED output durations
    unsigned int led_duration_dot;                      // mS
    unsigned int led_duration_dash;                     // mS
    unsigned int led_duration_element;                  // mS
    unsigned int led_duration_interelement;             // mS
    unsigned int led_duration_interletter;              // mS   actual (Farnsworth timing plus interval multiplication)
    unsigned int led_duration_interletter_fw;           // mS   Farnsworth timing
    unsigned int led_duration_interword;                // mS   actual (Farnsworth timing plus interval multiplication)
    unsigned int led_duration_interword_fw;             // mS   Farnsworth timing
    unsigned int led_duration_1min_check;               // mS

    unsigned int duration_follow_on;                    // mS
    unsigned int duration_intermessage;                 // mS

    unsigned int dit_samples;                   // Number of samples in a dit
    unsigned int dah_samples;                   // Number of samples in a dah
    unsigned int interelement_samples;          // Number of samples between elements
    unsigned int interletter_samples_fw;        // Number of samples between letters
    unsigned int interletter_samples;           // Number of samples between letters
    unsigned int interword_samples_fw;          // Number of samples between words
    unsigned int interword_samples;             // Number of samples between words

    unsigned int intermessage_samples;          // Number of samples between messages

    unsigned int cw_rising_edge_samples;        // Number of samples in a CW rising edge
    unsigned int cw_falling_edge_samples;       // Number of samples in a CW falling edge
    unsigned int cw_burst_noise_start_samples;  // Number of samples in burst of white noise at the start of a Morse dit or dah
    unsigned int cw_burst_noise_end_samples;    // Number of samples in burst of white noise at the end of a Morse dit or dah

    unsigned int bg_noise_fade_samples;         // Number of samples in background noise fade up/fade down

    unsigned int duration_tick;                 // mS
    unsigned int tick_samples;                  // Number of samples in a 20mS tick

    // Levels

    float cw_h1_level;                          // Normalised 1st harmonic level
    float cw_h2_level;                          //
    float cw_h3_level;                          //
    float cw_h4_level;                          //

    float cw_noise_level;                       //
    float cw_noise_burst_start_level;           //
    float cw_noise_burst_end_level;             //
    float bg_noise_level_active;                //
    float bg_noise_level_quiescent;             //

    bool  bg_noise;
    bool  bg_noise_fading;

};


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

protected:

    C_config( const C_config & ){}

    bool
    check_params( int argc, char *argv[] );

    void
    usage();

    bool
    read( string & filename, string & profile_list );

    void
    calculate_derived_settings();

    unsigned int
    calculate_element_duration( float morse_speed_char );

    void
    calculate_farnsworth_delays( float        morse_speed_char
                               , float        morse_speed_effective
                               , unsigned int & intercharacter_delay
                               , unsigned int & interword_delay );

    unsigned int
    calculate_check_1min_duration( unsigned int duration_element
                                 , unsigned int duration_interletter
                                 , unsigned int duration_interword
                                 , float        effective_char_speed );

    void
    display_settings();

    bool
    read_profile( const string & profile );

    bool
    cross_checks();

    void
    separate_path_and_list( const string &full_path, string &path, string &list );

    const char *
    to_string( bool setting );

    const char *
    to_string( eOption opt );

    void
    to_enum( const string & output_mode_s, eOutputMode & output_mode_e );

    const char *
    to_string( eOutputMode output_mode_e );

    void
    to_enum( const string & morse_mode_s, eMorseMode & morse_mode_e );

    const char *
    to_string( eMorseMode morse_mode_e );

    double
    lin_to_exp( double val );

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
