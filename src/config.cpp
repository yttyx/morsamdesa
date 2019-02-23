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

// config.cpp

#include <assert.h>
#include <cstddef>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "misc.h"

using namespace  morsamdesa;

namespace morsamdesa
{

C_config cfg;
extern C_log log;

static S_option options[] =
{
    { OPT_DISPLAY_VERBOSITY,        FLD_INTEGER,            offsetof( S_config, display_verbosity ),            "display.verbosity",                      "0",   "6"      },
    { OPT_DISPLAY_DATETIME,         FLD_BOOLEAN,            offsetof( S_config, display_datetime ),             "display.datetime",                       "",    ""       },
    { OPT_OUTPUT_MODE,              FLD_OUTPUT_MODE,        offsetof( S_config, output_mode ),                  "output.mode",                            "1",   "20"     },
    { OPT_OUTPUT_FILE,              FLD_STRING,             offsetof( S_config, output_file ),                  "output.file",                            "1",   "256"    },
    { OPT_OUTPUT_LED,               FLD_BOOLEAN,            offsetof( S_config, output_led ),                   "output.led",                             "false", "true" },
    { OPT_DELAY_FOLLOW_ON,          FLD_FLOAT,              offsetof( S_config, delay_follow_on ),              "delay.follow_on",                        "0.5",  "10"    },
    { OPT_DELAY_INTERMESSAGE,       FLD_FLOAT,              offsetof( S_config, delay_intermessage),            "delay.intermessage",                     "0.0", "30.0"   },
    { OPT_NIGHTMODE_START,          FLD_STRING,             offsetof( S_config, nightmode_start ),              "night_mode.start",                       "5",   "5"      },
    { OPT_NIGHTMODE_END,            FLD_STRING,             offsetof( S_config, nightmode_end ),                "night_mode.end",                         "5",   "5"      },
    { OPT_FEED_HEADLINES_ENABLED,   FLD_BOOLEAN,            offsetof( S_config, feed_headlines_enabled ),       "feed.headlines.enabled",                 "",    ""       },
    { OPT_FEED_HEADLINES_PREFIX,    FLD_BOOLEAN,            offsetof( S_config, feed_headlines_prefix ),        "feed.headlines.prefix",                  "",    ""       },
    { OPT_FEED_HEADLINES_URLS,      FLD_URL_ARRAY,          offsetof( S_config, feed_headlines_urls ),          "feed.headlines.urls",                    "0",   "0"      },
    { OPT_FEED_HEADLINES_DELAY_MIN, FLD_INTEGER,            offsetof( S_config, feed_headlines_delay_min ),     "feed.headlines.delay.min",               "120", "600"    },
    { OPT_FEED_HEADLINES_DELAY_MAX, FLD_INTEGER,            offsetof( S_config, feed_headlines_delay_max ),     "feed.headlines.delay.max",               "120", "600"    },
    { OPT_FEED_HEADLINES_FILTERS,   FLD_STRING_ARRAY,       offsetof( S_config, feed_headlines_filters ),       "feed.headlines.filters",                 "0",   "20"     },
    { OPT_FEED_TIME_ENABLED,        FLD_BOOLEAN,            offsetof( S_config, feed_time_enabled ),            "feed.time.enabled",                      "false", "true" },
    { OPT_FEED_TIME_INTERVAL,       FLD_INTEGER,            offsetof( S_config, feed_time_interval ),           "feed.time.interval",                     "10",  "60"     },
    { OPT_FEED_FIXED_ENABLED,       FLD_BOOLEAN,            offsetof( S_config, feed_fixed_enabled ),           "feed.fixed.enabled",                     "false", "true" },
    { OPT_FEED_FIXED_MESSAGE,       FLD_STRING,             offsetof( S_config, feed_fixed_message ),           "feed.fixed.message",                     "1",   "256"    },
    { OPT_FEED_WORDLIST_ENABLED,    FLD_BOOLEAN,            offsetof( S_config, feed_wordlist_enabled ),        "feed.wordlist.enabled",                  "false", "true" },
    { OPT_FEED_WORDLIST_REPEAT,     FLD_BOOLEAN,            offsetof( S_config, feed_wordlist_repeat ),         "feed.wordlist.repeat",                   "false", "true" },
    { OPT_FEED_WORDLIST_RANDOM,     FLD_BOOLEAN,            offsetof( S_config, feed_wordlist_random ),         "feed.wordlist.random",                   "false", "true" },
    { OPT_FEED_WORDLIST_FILE,       FLD_STRING,             offsetof( S_config, feed_wordlist_file ),           "feed.wordlist.file",                     "1",     "256"  },
    { OPT_FEED_QUEUE_UNHELD,        FLD_INTEGER,            offsetof( S_config, feed_queue_unheld ),            "feed.queue.unheld",                      "1",     "40"   },
    { OPT_NOISE_BG_FILE,            FLD_STRING,             offsetof( S_config, noise_bg_file ),                "noise.background.file",                  "0",   "255"    },
    { OPT_NOISE_BG_FADE_TIME,       FLD_FLOAT,              offsetof( S_config, noise_bg_fade_time ),           "noise.background.fade_time",             "0.0", "10.0"   },
    { OPT_NOISE_BG_LEVEL_QUIESCENT, FLD_LEVEL,              offsetof( S_config, noise_bg_level_quiescent ),     "noise.background.level.quiescent",       "0.0", "10.0"   },
    { OPT_NOISE_BG_LEVEL_ACTIVE,    FLD_LEVEL,              offsetof( S_config, noise_bg_level_active ),        "noise.background.level.active",          "0.0", "10.0"   },
    { OPT_TRANSMITTERS,             FLD_TRANSMITTER_ARRAY,  offsetof( S_config, transmitters ),                 "transmitters",                           "0",   "0"      },
    { OPT_NONE,                     FLD_NONE,               0,                                                  "",                                       "0",   "0"      }
};

static S_option url_options[] =
{
    { OPT_URL_MNEMONIC, FLD_STRING,  offsetof( S_url, mnemonic ), "mnemonic", "3", "3"   },
    { OPT_URL,          FLD_STRING,  offsetof( S_url, url ),      "url",      "0", "256" },
    { OPT_NONE,         FLD_NONE,    0,                           "",         "0",   "0" }
};

static S_option transmitter_options[] =
{
    { OPT_TRANSMITTER_MNEMONIC,           FLD_STRING,     offsetof( S_transmitter, mnemonic ),                      "mnemonic",                         "3",   "3"      },
    { OPT_CODE,                           FLD_MORSE_CODE, offsetof( S_transmitter, morse_code ),                    "code",                             "1",   "20"     },
    { OPT_MODE,                           FLD_MORSE_MODE, offsetof( S_transmitter, morse_mode ),                    "mode",                             "1",   "20"     },
    { OPT_ALPHANUMERIC_ONLY,              FLD_BOOLEAN,    offsetof( S_transmitter, alphanumeric_only ),             "alphanumeric_only",                "false", "true" },
    { OPT_LEVEL,                          FLD_LEVEL,      offsetof( S_transmitter, level ),                         "level",                            "0.0", "10.0"   },
    { OPT_SPEED_AUDIO_CHAR,               FLD_FLOAT,      offsetof( S_transmitter, speed_char ),                    "speed.character",                  "1",   "60"     },
    { OPT_SPEED_AUDIO_EFFECTIVE,          FLD_FLOAT,      offsetof( S_transmitter, speed_effective ),               "speed.effective",                  "1",   "60"     },
    { OPT_INTERVAL_MULT_CHAR,             FLD_FLOAT,      offsetof( S_transmitter, interval_multiplier_char ),      "interval_multiplier.character",    "1.0", "10.0"   },
    { OPT_INTERVAL_MULT_WORD,             FLD_FLOAT,      offsetof( S_transmitter, interval_multiplier_word ),      "interval_multiplier.word",         "1.0", "10.0"   },
    { OPT_SOUNDER_ARM_DOWN,               FLD_STRING,     offsetof( S_transmitter, sounder_arm_down ),              "sounder.arm_down",                 "0",   "255"    },
    { OPT_SOUNDER_ARM_UP,                 FLD_STRING,     offsetof( S_transmitter, sounder_arm_up ),                "sounder.arm_up",                   "0",   "255"    },
    { OPT_SOUNDER_HI,                     FLD_STRING,     offsetof( S_transmitter, sounder_hi ),                    "sounder.hi",                       "0",   "255"    },
    { OPT_SOUNDER_LO,                     FLD_STRING,     offsetof( S_transmitter, sounder_lo ),                    "sounder.lo",                       "0",   "255"    },
    { OPT_SPARKGAP_DOT,                   FLD_STRING,     offsetof( S_transmitter, sparkgap_dot ),                  "sparkgap.dot",                     "0",   "255"    },
    { OPT_SPARKGAP_DASH,                  FLD_STRING,     offsetof( S_transmitter, sparkgap_dash ),                 "sparkgap.dash",                    "0",   "255"    },
    { OPT_CW_H1_FREQUENCY,                FLD_INTEGER,    offsetof( S_transmitter, cw_h1_frequency ),               "cw.harmonic.h1.frequency",         "100", "5000"   },
    { OPT_CW_H1_LEVEL,                    FLD_LEVEL,      offsetof( S_transmitter, cw_h1_level ),                   "cw.harmonic.h1.level",             "0.0", "10.0"   },
    { OPT_CW_H2_MULT,                     FLD_FLOAT,      offsetof( S_transmitter, cw_h2_multiplier ),              "cw.harmonic.h2.multiplier",        "0.0", "16.0"   },
    { OPT_CW_H2_LEVEL,                    FLD_LEVEL,      offsetof( S_transmitter, cw_h2_level ),                   "cw.harmonic.h2.level",             "0.0", "10.0"   },
    { OPT_CW_H3_MULT,                     FLD_FLOAT,      offsetof( S_transmitter, cw_h3_multiplier ),              "cw.harmonic.h3.multiplier",        "0.0", "16.0"   },
    { OPT_CW_H3_LEVEL,                    FLD_LEVEL,      offsetof( S_transmitter, cw_h3_level ),                   "cw.harmonic.h3.level",             "0.0", "10.0"   },
    { OPT_CW_H4_MULT,                     FLD_FLOAT,      offsetof( S_transmitter, cw_h4_multiplier ),              "cw.harmonic.h4.multiplier",        "0.0", "16.0"   },
    { OPT_CW_H4_LEVEL,                    FLD_LEVEL,      offsetof( S_transmitter, cw_h4_level ),                   "cw.harmonic.h4.level",             "0.0", "10.0"   },
    { OPT_CW_RISE_TIME,                   FLD_FLOAT,      offsetof( S_transmitter, cw_rise_time ),                  "cw.rise_time",                     "1.0", "20.0"   },
    { OPT_CW_FALL_TIME,                   FLD_FLOAT,      offsetof( S_transmitter, cw_fall_time ),                  "cw.fall_time",                     "1.0", "20.0"   },
    { OPT_CW_NOISE_BURST_START_DURATION,  FLD_FLOAT,      offsetof( S_transmitter, cw_noise_burst_start_duration ), "cw.noise.burst.start.duration",    "0.0", "5.0"    },
    { OPT_CW_NOISE_BURST_START_LEVEL,     FLD_LEVEL,      offsetof( S_transmitter, cw_noise_burst_start_level ),    "cw.noise.burst.start.level",       "0.0", "10.0"   },
    { OPT_CW_NOISE_BURST_END_DURATION,    FLD_FLOAT,      offsetof( S_transmitter, cw_noise_burst_end_duration ),   "cw.noise.burst.end.duration",      "0.0", "5.0"    },
    { OPT_CW_NOISE_BURST_END_LEVEL,       FLD_LEVEL,      offsetof( S_transmitter, cw_noise_burst_end_level ),      "cw.noise.burst.end.level",         "0.0", "10.0"   },
    { OPT_NONE,                           FLD_NONE,       0,                                                        "",                                 "0",   "0"      }
};

const char *
to_string( eOutputMode output_mode )
{
    switch ( output_mode )
    {
        case OMODE_PULSEAUDIO:
            return "pulseaudio";
        case OMODE_MP3:
            return "mp3";
        default:
            return "invalid";
    }

    return "?";
}

const char *
to_string( eMorseCode morse_code )
{
    switch ( morse_code )
    {
        case MC_INTERNATIONAL:
            return "international";
        case MC_AMERICAN:
            return "american";
        default:
            return "invalid";
    }

    return "?";
}

const char *
to_string( eMorseMode morse_mode )
{
    switch ( morse_mode )
    {
        case MM_CW:
            return "cw";
        case MM_DOUBLE_PLATE_SOUNDER:
            return "double_plate_sounder";
        case MM_LED:
            return "led";
        case MM_SOUNDER:
            return "sounder";
        case MM_SPARKGAP:
            return "sparkgap";
        default:
            return "invalid";
    }

    return "?";
}


C_config::C_config()
{
}

C_config::~C_config()
{
}

bool
C_config::read( int argc, char *argv[] )
{
    if ( ! check_params( argc, argv ) )
    {
        usage();
        return false;
    }

    // Try to read a configuration if one was supplied, otherwise use a default configuration
    if ( got_config_file_ )
    {
        if ( ! read( config_file_, profile_list_ ) )
        {
            log_writeln( C_log::LL_ERROR, "Configuration error" );
            return false;
        }
    }

    if ( display_config_ )
    {
        display_settings();
        return false;
    }

    return true;
}

bool
C_config::read( string & filename, string & profile_list )
{
    try
    {
        cfg_.readFile( filename.c_str() );
    }
    catch ( const FileIOException &fioex )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Error reading configuration file '%s'", filename.c_str() );
        return false;
    }
    catch ( const ParseException &pex )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Error in configuration file '%s' at line %d: %s", pex.getFile(), pex.getLine(), pex.getError() );
        return false;
    }

    // Read base profile
    bool worked = read_profile( "standard" );

    if ( profile_list.length() > 0 )
    {
        istringstream profile_ss( profile_list );
        string        profile;

        // Apply each specified profile onto the base profile
        while ( worked && ( getline( profile_ss, profile, ';' ) ) )
        {
            worked = worked && read_profile( profile );
        }
    }

    worked = worked && setting_checks();
    worked = worked && calculate_derived_settings();

    return worked;
}

bool
C_config::read_profile( const string & profile )
{
    // Check that profile exists

    string field_path;

    field_path = C_log::format_string( "morsamdesa.profile.%s", profile.c_str() );

    if ( ! cfg_.exists( field_path ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Profile '%s' does not exist in configuration file", profile.c_str() );
        return false;
    }

    const Setting & profile_settings = cfg_.lookup( field_path.c_str() );

    read_option_table( profile_settings, options, &config_ );

    // Set logging parameters
    log.initialise( cfg.c().display_verbosity, cfg.c().display_datetime );

    log_writeln_fmt( C_log::LL_INFO, "Applied profile: %s", profile.c_str() );

    return true;
}

void
C_config::read_option_table( const Setting & parent, const S_option * options, void * record )
{
    float level = 0.0;

    // Parse profile settings
    for ( ; options->opt != OPT_NONE; options++ )
    {
        try
        {
            switch ( options->field_type )
            {
                case FLD_STRING:
                    * ( ( string * ) ( ( ( char * ) record ) + options->field_offset ) ) = parent.lookup( options->field_path ).c_str();
                    break;

                case FLD_INTEGER:
                    * ( ( int * ) ( ( ( char * ) record ) + options->field_offset ) ) = parent.lookup( options->field_path );
                    break;

                case FLD_FLOAT:
                    * ( ( float * ) ( ( ( char * ) record ) + options->field_offset ) ) = parent.lookup( options->field_path );
                    break;

                case FLD_LEVEL:
                    level = parent.lookup( options->field_path );
                    * ( ( float * ) ( ( ( char * ) record ) + options->field_offset ) ) = level / 10.0;
                    break;

                case FLD_BOOLEAN:
                    * ( ( bool * ) ( ( ( char * ) record ) + options->field_offset ) ) = parent.lookup( options->field_path );
                    break;

                case FLD_STRING_ARRAY:
                {
                    const Setting &items = parent.lookup( options->field_path );
                    int   item_count     = items.getLength();

                    for ( int ii = 0; ii < item_count; ii++ )
                    {
                        string str = items[ ii ];

                        ( ( vector< string > & )( * ( ( ( char * ) record ) + options->field_offset ) ) ).push_back( str );
                    }
                    break;
                }

                case FLD_URL_ARRAY:
                    url_array( parent, options->field_path, ( vector< S_url > & )( * ( ( ( char * ) &config_ ) + options->field_offset ) ) );
                    break;

                case FLD_TRANSMITTER_ARRAY:
                    transmitter_array( parent, options->field_path, ( vector< S_transmitter > & )( * ( ( ( char * ) &config_ ) + options->field_offset ) ) );
                    break;

                case FLD_MORSE_CODE:
                    eMorseCode morse_code;
                    
                    to_enum( parent.lookup( options->field_path ).c_str(), morse_code );
                    * ( ( eMorseCode * ) ( ( ( char * ) record ) + options->field_offset ) ) = morse_code;
                    break;

                case FLD_MORSE_MODE:
                    eMorseMode morse_mode;
                    
                    to_enum( parent.lookup( options->field_path ).c_str(), morse_mode );
                    * ( ( eMorseMode * ) ( ( ( char * ) record ) + options->field_offset ) ) = morse_mode;
                    break;

                case FLD_OUTPUT_MODE:
                    eOutputMode output_mode;
                    
                    to_enum( parent.lookup( options->field_path ).c_str(), output_mode );
                    * ( ( eOutputMode * ) ( ( ( char * ) record ) + options->field_offset ) ) = output_mode;
                    break;

                default:
                    break;
            }
        }
        catch ( const SettingNotFoundException & nfex )
        {
            log_writeln_fmt( C_log::LL_VERBOSE_3, "Missing setting in configuration file: %s", nfex.getPath() );
        }
        catch ( const SettingTypeException & nfex )
        {
            log_writeln_fmt( C_log::LL_ERROR, "Incorrect parameter type in configuration file: %s", nfex.getPath() );
        }
        catch ( ... )
        {
            log_writeln( C_log::LL_ERROR, "EXCEPTION" );
        }
    }
}

void
C_config::url_array( const Setting & parent, const char * field_name, vector< S_url > & url_array )
{
    const Setting & urls = parent.lookup( field_name );
    int   urls_count     = urls.getLength();

    for ( int ii = 0; ii < urls_count; ii++ )
    {
        S_url this_url;

        const Setting & url = urls[ ii ];

        read_option_table( url, url_options, &this_url );

        url_array.push_back( this_url );
    }
}

void
C_config::transmitter_array( const Setting & parent, const char * field_name, vector< S_transmitter > & transmitter_array )
{
    const Setting & transmitters = parent.lookup( field_name );
    int   transmitters_count     = transmitters.getLength();

    for ( int ii = 0; ii < transmitters_count; ii++ )
    {
        S_transmitter this_transmitter;

        const Setting & transmitter = transmitters[ ii ];

        read_option_table( transmitter, transmitter_options, &this_transmitter );

        transmitter_array.push_back( this_transmitter );
    }
}

bool
C_config::setting_checks()
{
    return setting_checks( options, ( void * ) &config_ );
}

bool
C_config::setting_checks( const S_option * options, void * record )
{
    bool ok = true;

    for ( ; options->opt != OPT_NONE; options++ )
    {
        try
        {
            switch ( options->field_type )
            {
                case FLD_STRING:
                {
                    string str = * ( ( string * ) ( ( ( char * ) record ) + options->field_offset ) );
                    
                    if ( ( str.length() < ( size_t ) atoi( options->min ) ) || ( str.length() > ( size_t ) atoi( options->max ) ) )
                    {
                        log_writeln_fmt( C_log::LL_ERROR, "Invalid length: %s - %s", options->field_path, str.c_str() );
                        ok = false;
                    }
                    break;
                }
                case FLD_INTEGER:
                {
                    int i = * ( ( int * ) ( ( ( char * ) record ) + options->field_offset ) );

                    if ( ( i < atoi( options->min ) ) || ( i > atoi( options->max ) ) )
                    {
                        log_writeln_fmt( C_log::LL_ERROR, "Value out of range: %s - %d", options->field_path, i );
                        ok = false;
                    }
                    break;
                }
                case FLD_FLOAT:
                {
                    float f = * ( ( float * ) ( ( ( char * ) record ) + options->field_offset ) );

                    if ( ( f < atof( options->min ) ) || ( f > atof( options->max ) ) )
                    {
                        log_writeln_fmt( C_log::LL_ERROR, "Value out of range: %s - %f", options->field_path, f );
                        ok = false;
                    }
                    break;
                }
                case FLD_BOOLEAN:
                {
                    break;
                }
                case FLD_STRING_ARRAY:
                {
                    vector< string > & array = ( vector< string > & )( * ( ( ( char * ) record ) + options->field_offset ) );

                    for ( unsigned int ii = 0; ii < array.size(); ii++ )
                    {
                        string str = array[ ii ];

                        if ( ( str.length() < ( size_t ) atoi( options->min ) ) || ( str.length() > ( size_t ) atoi( options->max ) ) )
                        {
                            log_writeln_fmt( C_log::LL_ERROR, "Invalid length: %s - %s", options->field_path, str.c_str() );
                            ok = false;
                        }
                    }
                    break;
                }
                case FLD_URL_ARRAY:
                {
                    setting_checks( transmitter_options, ( vector< S_url > & )( * ( ( ( char * ) record ) + options->field_offset ) ) );
                    break;
                }
                case FLD_TRANSMITTER_ARRAY:
                {
                    setting_checks( transmitter_options, ( vector< S_transmitter > & )( * ( ( ( char * ) record ) + options->field_offset ) ) );
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        catch ( const exception & ex )
        {
            log_writeln_fmt( C_log::LL_ERROR, "Exception in parameter checks: %s", ex.what() );
        }
        catch ( ... )
        {
            log_writeln( C_log::LL_ERROR, "Exception in parameter checks" );
        }
    }

    // Cross-parameter checks
    if ( config_.feed_headlines_enabled && ( config_.feed_headlines_delay_min > config_.feed_headlines_delay_max ) )
    {
        log_writeln( C_log::LL_ERROR, "feed.headlines.delay.min must be <= than feed_.headlines.delay.max" );
        ok = false;
    }
    if ( config_.feed_headlines_enabled && ( config_.feed_headlines_urls.size() == 0 ) )
    {
        log_writeln( C_log::LL_ERROR, "No headline feed URLs were supplied" );
        ok = false;
    }
    if ( ! ( config_.feed_fixed_enabled || config_.feed_headlines_enabled || config_.feed_time_enabled || config_.feed_wordlist_enabled ) )
    {
        log_writeln( C_log::LL_ERROR, "No data feeds are enabled" );
        ok = false;
    }

    if ( ! valid_time( config_.nightmode_start ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Invalid night mode start time '%s'", config_.nightmode_start.c_str() );
    }
    if ( ! valid_time( config_.nightmode_end ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Invalid night mode end time '%s'", config_.nightmode_end.c_str() );
    }

    for ( unsigned int ii = 0; ii < config_.transmitters.size() ; ii++ )
    {
        S_transmitter trx = config_.transmitters[ ii ];

        if ( trx.speed_char < trx.speed_effective )
        {
            log_writeln( C_log::LL_ERROR, "speed.char >= speed.effective" );
            ok = false;
        }

        if ( ( trx.morse_code == MC_AMERICAN ) && ( trx.morse_mode == MM_DOUBLE_PLATE_SOUNDER ) )
        {
            log_writeln( C_log::LL_ERROR, "Double plate sounder output is not supported with American Morse code" );
            ok = false;
        }
    }

    return ok;
}

bool
C_config::setting_checks( const S_option * options, vector< S_url > & urls )
{
    bool ok = true;

    for ( unsigned int ii = 0; ii < urls.size() ; ii++ )
    {
        S_url url = urls[ ii ];

        if ( ! setting_checks( url_options, ( void * ) &url ) )
        {
            ok = false;
        }
    }

    return ok;
}

bool
C_config::setting_checks( const S_option * options, vector< S_transmitter > & transmitters )
{
    bool ok = true;

    for ( unsigned int ii = 0; ii < transmitters.size() ; ii++ )
    {
        S_transmitter trx = transmitters[ ii ];

        if ( ! setting_checks( transmitter_options, ( void * ) &trx ) )
        {
            ok = false;
        }
    }

    return ok;
}

bool
C_config::calculate_derived_settings()
{
    // Time used for short delays mS
    derived_.duration_tick                 = 20;

    derived_.duration_follow_on            = ( unsigned int ) ( config_.delay_follow_on * 1000.0 );      // mS
    derived_.duration_intermessage         = ( unsigned int ) ( config_.delay_intermessage * 1000.0 );   // mS

    // Convert durations to sample counts
    derived_.intermessage_samples          = SAMPLE_RATE * config_.delay_intermessage;

    derived_.bg_noise_fade_samples         = SAMPLE_RATE * config_.noise_bg_fade_time;
    derived_.tick_samples                  = ( SAMPLE_RATE * derived_.duration_tick ) / 1000;

    derived_.bg_noise_level_active         = config_.noise_bg_level_active / 10.0;
    derived_.bg_noise_level_quiescent      = config_.noise_bg_level_quiescent / 10.0;

    derived_.bg_noise                      = ( derived_.bg_noise_level_active > 0.0 );
    derived_.bg_noise_fading               = ( derived_.bg_noise_level_active > derived_.bg_noise_level_quiescent ) &&
                                             ( config_.noise_bg_fade_time > 0.0 );
    return true;
}

void
C_config::separate_path_and_list( const string & full_path, string & path, string & list )
{
    // NB: we make the assumption that the only list in the path is the final element of the path
    //     e.g. for the path 'rss.filters', 'filters' is a list.
    int dot_pos = full_path.find_last_of( '.' );

    path = full_path.substr( 0, dot_pos );
    list = full_path.substr( dot_pos + 1 );
}

void
C_config::display_settings()
{
    log_writeln( C_log::LL_INFO, "Morsamdesa configuration");
    log_writeln( C_log::LL_INFO, "----------------------------------------------------------------------------------------");

    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %d",    to_string( OPT_DISPLAY_VERBOSITY ),      config_.display_verbosity );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_DISPLAY_DATETIME ),       to_string( config_.display_datetime ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_OUTPUT_MODE ),            to_string( config_.output_mode ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_OUTPUT_FILE ),            config_.output_file.c_str() );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_OUTPUT_LED ),             to_string( config_.output_led ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_DELAY_FOLLOW_ON ),        config_.delay_follow_on );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_DELAY_INTERMESSAGE ),     config_.delay_intermessage );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_HEADLINES_ENABLED ), to_string( config_.feed_headlines_enabled ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_HEADLINES_PREFIX ),  to_string( config_.feed_headlines_prefix ) );
    log_writeln( C_log::LL_INFO, "--");

    for ( unsigned int ii = 0; ii < config_.feed_headlines_urls.size() ; ii++ )
    {
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s", to_string( OPT_FEED_HEADLINES_URLS, OPT_URL_MNEMONIC ).c_str(), config_.feed_headlines_urls[ ii ].mnemonic.c_str() );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s", to_string( OPT_FEED_HEADLINES_URLS, OPT_URL).c_str(),           config_.feed_headlines_urls[ ii ].url.c_str()      );
        log_writeln( C_log::LL_INFO, "--");
    }

    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %d",    to_string( OPT_FEED_HEADLINES_DELAY_MIN ), config_.feed_headlines_delay_min );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %d",    to_string( OPT_FEED_HEADLINES_DELAY_MAX ), config_.feed_headlines_delay_max );

    unsigned int filter_count = config_.feed_headlines_filters.size();

    for ( unsigned int ii = 0; ii < filter_count; ii++ )
    {
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s", to_string( OPT_FEED_HEADLINES_FILTERS ),  config_.feed_headlines_filters[ ii ].c_str() );
    }

    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_TIME_ENABLED ),        to_string( config_.feed_time_enabled ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %d",    to_string( OPT_FEED_TIME_INTERVAL ),       config_.feed_time_interval );

    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_FIXED_ENABLED ),       to_string( config_.feed_fixed_enabled ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_FIXED_MESSAGE ),       config_.feed_fixed_message.c_str() );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %d",    to_string( OPT_FEED_QUEUE_UNHELD ),        config_.feed_queue_unheld );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_WORDLIST_ENABLED ),    to_string( config_.feed_wordlist_enabled ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_WORDLIST_RANDOM ),     to_string( config_.feed_wordlist_random ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_WORDLIST_REPEAT ),     to_string( config_.feed_wordlist_repeat ) );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_FEED_WORDLIST_FILE ),       config_.feed_wordlist_file.c_str() );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_NIGHTMODE_START ),          config_.nightmode_start.c_str() );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_NIGHTMODE_END ),            config_.nightmode_end.c_str() );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_NOISE_BG_FILE ),            config_.noise_bg_file.c_str() );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_NOISE_BG_FADE_TIME ),       config_.noise_bg_fade_time );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_NOISE_BG_LEVEL_QUIESCENT ), config_.noise_bg_level_quiescent );
    log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_NOISE_BG_LEVEL_ACTIVE ),    config_.noise_bg_level_active );

    for ( unsigned int ii = 0; ii < config_.transmitters.size() ; ii++ )
    {
        S_transmitter trx = config_.transmitters[ ii ];

        log_writeln( C_log::LL_INFO, "--");
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_TRANSMITTER_MNEMONIC ).c_str(),          trx.mnemonic.c_str() );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_MODE ).c_str(),                          to_string( trx.morse_mode ) );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_CODE ).c_str(),                          to_string( trx.morse_code ) );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_ALPHANUMERIC_ONLY ).c_str(),             to_string( trx.alphanumeric_only ) );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_LEVEL ).c_str(),                         trx.level );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_SPEED_AUDIO_CHAR ).c_str(),              trx.speed_char );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_SPEED_AUDIO_EFFECTIVE ).c_str(),         trx.speed_effective );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_INTERVAL_MULT_CHAR ).c_str(),            trx.interval_multiplier_char );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_INTERVAL_MULT_WORD ).c_str(),            trx.interval_multiplier_word );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_SOUNDER_ARM_DOWN ).c_str(),              trx.sounder_arm_down.c_str() );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_SOUNDER_ARM_UP ).c_str(),                trx.sounder_arm_up.c_str() );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_SOUNDER_HI ).c_str(),                    trx.sounder_hi.c_str() );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_SOUNDER_LO ).c_str(),                    trx.sounder_lo.c_str() );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_SPARKGAP_DOT ).c_str(),                  trx.sparkgap_dot.c_str() );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %s",    to_string( OPT_TRANSMITTERS, OPT_SPARKGAP_DASH ).c_str(),                 trx.sparkgap_dash.c_str() );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %d",    to_string( OPT_TRANSMITTERS, OPT_CW_H1_FREQUENCY ).c_str(),               trx.cw_h1_frequency );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_H1_LEVEL ).c_str(),                   trx.cw_h1_level );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_H2_MULT ).c_str(),                    trx.cw_h2_multiplier );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_H2_LEVEL ).c_str(),                   trx.cw_h2_level );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_H3_MULT ).c_str(),                    trx.cw_h3_multiplier );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_H3_LEVEL ).c_str(),                   trx.cw_h3_level );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_H4_MULT ).c_str(),                    trx.cw_h4_multiplier );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_H4_LEVEL ).c_str(),                   trx.cw_h4_level );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_RISE_TIME ).c_str(),                  trx.cw_rise_time );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_FALL_TIME ).c_str(),                  trx.cw_fall_time );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_NOISE_BURST_START_LEVEL ).c_str(),    trx.cw_noise_burst_start_level );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_NOISE_BURST_START_DURATION ).c_str(), trx.cw_noise_burst_start_duration );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_NOISE_BURST_END_LEVEL ).c_str(),      trx.cw_noise_burst_end_level );
        log_writeln_fmt( C_log::LL_INFO, "%-43.43s: %1.3f", to_string( OPT_TRANSMITTERS, OPT_CW_NOISE_BURST_END_DURATION ).c_str(),   trx.cw_noise_burst_end_duration );
    }

    log_writeln( C_log::LL_INFO,     "------------------------------------------------------------------------------" );
    log_writeln( C_log::LL_INFO,     "Derived settings" );
    log_writeln( C_log::LL_INFO,     "------------------------------------------------------------------------------" );

    log_writeln( C_log::LL_INFO, "" );
    log_writeln_fmt( C_log::LL_INFO, "  Tick duration                     : %5u mS", derived_.duration_tick                );
    log_writeln_fmt( C_log::LL_INFO, "  Tick samples                      : %6u",    derived_.tick_samples                 );
    log_writeln_fmt( C_log::LL_INFO, "  Follow on duration                : %5u mS", derived_.duration_follow_on           );
    log_writeln_fmt( C_log::LL_INFO, "  Intermessage duration             : %5u mS", derived_.duration_intermessage        );
    log_writeln( C_log::LL_INFO, "" );
    log_writeln_fmt( C_log::LL_INFO, "  Background noise enabled          : %s",     to_string( derived_.bg_noise )        );
    log_writeln_fmt( C_log::LL_INFO, "  Background noise fading enabled   : %s",     to_string( derived_.bg_noise_fading ) );
    log_writeln_fmt( C_log::LL_INFO, "  Background noise fade samples     : %6u",    derived_.bg_noise_fade_samples        );
    log_writeln( C_log::LL_INFO, "" );
    log_writeln_fmt( C_log::LL_INFO, "  Background noise active level     : %1.3f",  derived_.bg_noise_level_active        );
    log_writeln_fmt( C_log::LL_INFO, "  Background noise quiescent level  : %1.3f",  derived_.bg_noise_level_quiescent     );
}

const char *
C_config::to_string( bool setting )
{
    return setting ? "true" : "false";
}

string
C_config::to_string( eOption opt1, eOption opt2 )
{
    return C_log::format_string( "%s.%s", to_string( opt1 ), to_string( opt2 ) );
}

const char *
C_config::to_string( eOption opt )
{
    // Search each of the options tables until the text related to opt is found
    for ( S_option *op = options; op->opt != OPT_NONE; op++ )
    {
        if ( op->opt == opt )
        {
            return op->field_path;
        }
    }

    for ( S_option *op = url_options; op->opt != OPT_NONE; op++ )
    {
        if ( op->opt == opt )
        {
            return op->field_path;
        }
    }

    for ( S_option *op = transmitter_options; op->opt != OPT_NONE; op++ )
    {
        if ( op->opt == opt )
        {
            return op->field_path;
        }
    }

    return "";
}

void
C_config::to_enum( const string & output_mode_s, eOutputMode & output_mode_e )
{
    output_mode_e = OMODE_INVALID;

    string s = output_mode_s;

    if ( s.compare( "pulseaudio" ) == 0 )
    {
       output_mode_e = OMODE_PULSEAUDIO;
    }
    else if ( s.compare( "mp3" ) == 0 )
    {
       output_mode_e = OMODE_MP3;
    }
}

void
C_config::to_enum( const string & morse_code_s, eMorseCode & morse_code_e )
{
    morse_code_e = MC_INVALID;

    string s = morse_code_s;

    if ( s.compare( "international" ) == 0 )
    {
       morse_code_e = MC_INTERNATIONAL;
    }
    else if ( s.compare( "american" ) == 0 )
    {
       morse_code_e = MC_AMERICAN;
    }
}

void
C_config::to_enum( const string & morse_mode_s, eMorseMode & morse_mode_e )
{
    morse_mode_e = MM_INVALID;

    string s = morse_mode_s;

    if ( s.compare( "cw" ) == 0 )
    {
       morse_mode_e = MM_CW;
    }
    else if ( s.compare( "led" ) == 0 )
    {
       morse_mode_e = MM_LED;
    }
    else if ( s.compare( "double_plate_sounder" ) == 0 )
    {
       morse_mode_e = MM_DOUBLE_PLATE_SOUNDER;
    }
    else if ( s.compare( "sounder" ) == 0 )
    {
       morse_mode_e = MM_SOUNDER;
    }
    else if ( s.compare( "sparkgap" ) == 0 )
    {
       morse_mode_e = MM_SPARKGAP;
    }
}

// Expected format is HH:MM
bool
C_config::valid_time( string & time )
{
    if ( time.compare( "00:00" ) == 0 )
    {
        return true;
    }

    struct tm tm_time;

    return strptime( time.c_str(), "%H:%M", &tm_time ) != NULL;
}

bool
C_config::check_params( int argc, char *argv[] )
{
    int opt;

    while ( ( opt = getopt( argc, argv, "c:p:dh?" ) ) != -1 )
    {
        switch ( opt )
        {
            case 'c':
                config_file_ = optarg;
                got_config_file_ = true;
                break;

            case 'd':
                display_config_ = true;
                break;

            case 'p':
                if ( profile_list_.length() > 0 )
                {
                    profile_list_ += ";";
                }
                profile_list_ += optarg;
                got_profile_list_ = true;
                break;

            case '?':
            case 'h':
            default:
                return false;
        }
    }

    if ( got_profile_list_ && ( ! got_config_file_ ) )
    {
        return false;
    }

    return true;
}

void
C_config::usage()
{
    log_writeln( C_log::LL_INFO, "morsamdesa [-c <configuration file>] [-p <profile name>] [-p <profile name>] [-d] [-h] [-?]" );
    log_writeln( C_log::LL_INFO, "morsamdesa  -d: display configuration settings" );
    log_writeln( C_log::LL_INFO, "morsamdesa  -h: display valid options" );
    log_writeln( C_log::LL_INFO, "morsamdesa  -p: apply named profile to base configuration" );
    log_writeln( C_log::LL_INFO, "morsamdesa  -?: display valid options" );
}

}
