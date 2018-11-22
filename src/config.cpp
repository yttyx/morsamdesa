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


struct output_mode
{
    const char  *output_mode;
    eOutputMode output_mode_e;
};

static output_mode output_modes[] =
{
    { "pulseaudio", OMODE_PULSEAUDIO    },
    { "mp3",        OMODE_MP3           },
    { NULL,         OMODE_INVALID       }
};

struct morse_mode
{
    const char  *morse_mode;
    eMorseMode  morse_mode_e;
};

static morse_mode morse_modes[] =
{
    { "sounder", MM_SOUNDER     },
    { "cw",      MM_CW          },
    { NULL,      MM_INVALID     }
};

struct S_option
{
    eOption       opt;
    Setting::Type field_type;
    int           field_offset;
    const char    *field_path;
    const char    *min;
    const char    *max;
};

static S_option options[] =
{
    { OPT_DISPLAY_VERBOSITY,                    Setting::TypeInt,     offsetof( S_config, display_verbosity ),                   "display.verbosity",                      "0",   "6"      },
    { OPT_DISPLAY_DATETIME,                     Setting::TypeBoolean, offsetof( S_config, display_datetime ),                    "display.datetime",                       "",    ""       },
    { OPT_OUTPUT_MODE,                          Setting::TypeString,  offsetof( S_config, output_mode ),                         "output.mode",                            "1",   "20"     },
    { OPT_OUTPUT_FILE,                          Setting::TypeString,  offsetof( S_config, output_file ),                         "output.file",                            "1",   "256"    },
    { OPT_REMOTE_ENABLED,                       Setting::TypeBoolean, offsetof( S_config, remote_enabled ),                      "remote.enabled",                         "",    ""       },
    { OPT_FEED_HEADLINES_ENABLED,               Setting::TypeBoolean, offsetof( S_config, feed_headlines_enabled ),              "feed.headlines.enabled",                 "",    ""       },
    { OPT_FEED_HEADLINES_URLS,                  Setting::TypeArray,   offsetof( S_config, feed_headlines_urls ),                 "feed.headlines.urls",                    "0",   "256"    },
    { OPT_FEED_HEADLINES_DELAY_MIN,             Setting::TypeInt,     offsetof( S_config, feed_headlines_delay_min ),            "feed.headlines.delay.min",               "60",  "600"    },
    { OPT_FEED_HEADLINES_DELAY_MAX,             Setting::TypeInt,     offsetof( S_config, feed_headlines_delay_max ),            "feed.headlines.delay.max",               "60",  "600"    },
    { OPT_FEED_HEADLINES_FILTERS,               Setting::TypeArray,   offsetof( S_config, feed_headlines_filters ),              "feed.headlines.filters",                 "0",   "20"     },
    { OPT_FEED_TIME_ENABLED,                    Setting::TypeBoolean, offsetof( S_config, feed_time_enabled ),                   "feed.time.enabled",                      "false", "true" },
    { OPT_FEED_TIME_INTERVAL,                   Setting::TypeInt,     offsetof( S_config, feed_time_interval ),                  "feed.time.interval",                     "10",  "60"     },
    { OPT_FEED_FIXED_ENABLED,                   Setting::TypeBoolean, offsetof( S_config, feed_fixed_enabled ),                  "feed.fixed.enabled",                     "false", "true" },
    { OPT_FEED_FIXED_MESSAGE,                   Setting::TypeString,  offsetof( S_config, feed_fixed_message ),                  "feed.fixed.message",                     "1",   "256"    },
    { OPT_FEED_WORDLIST_ENABLED,                Setting::TypeBoolean, offsetof( S_config, feed_wordlist_enabled ),               "feed.wordlist.enabled",                  "false", "true" },
    { OPT_FEED_WORDLIST_REPEAT,                 Setting::TypeBoolean, offsetof( S_config, feed_wordlist_repeat ),                "feed.wordlist.repeat",                   "false", "true" },
    { OPT_FEED_WORDLIST_RANDOM,                 Setting::TypeBoolean, offsetof( S_config, feed_wordlist_random ),                "feed.wordlist.random",                   "false", "true" },
    { OPT_FEED_WORDLIST_FILE,                   Setting::TypeString,  offsetof( S_config, feed_wordlist_file ),                  "feed.wordlist.file",                     "1",     "256"  },
    { OPT_FEED_QUEUE_LENGTH,                    Setting::TypeInt,     offsetof( S_config, feed_queue_length ),                   "feed.queue.length",                      "8",     "40"   },
    { OPT_FEED_QUEUE_UNHELD,                    Setting::TypeInt,     offsetof( S_config, feed_queue_unheld ),                   "feed.queue.unheld",                      "1",     "40"   },
    { OPT_FEED_QUEUE_DISCARDABLE,               Setting::TypeInt,     offsetof( S_config, feed_queue_discardable ),              "feed.queue.discardable",                 "1",     "40"   },
    { OPT_MORSE_MODE,                           Setting::TypeString,  offsetof( S_config, morse_mode ),                          "morse.mode",                             "1",   "10"     },
    { OPT_MORSE_LED_ENABLED,                    Setting::TypeBoolean, offsetof( S_config, morse_led_enabled ),                   "morse.led.enabled",                      "false", "true" },
    { OPT_MORSE_NIGHTMODE_START,                Setting::TypeString,  offsetof( S_config, morse_nightmode_start ),               "morse.night_mode.start",                 "5",   "5"      },
    { OPT_MORSE_NIGHTMODE_END,                  Setting::TypeString,  offsetof( S_config, morse_nightmode_end ),                 "morse.night_mode.end",                   "5",   "5"      },
    { OPT_MORSE_ALPHANUMERIC_ONLY,              Setting::TypeBoolean, offsetof( S_config, morse_alphanumeric_only ),             "morse.alphanumeric_only",                "false", "true" },
    { OPT_MORSE_DELAY_FOLLOW_ON,                Setting::TypeFloat,   offsetof( S_config, morse_delay_follow_on ),               "morse.delay.follow_on",                  "0.5",  "10"    },
    { OPT_MORSE_DELAY_INTERMESSAGE,             Setting::TypeFloat,   offsetof( S_config, morse_delay_intermessage),             "morse.delay.intermessage",               "0.0", "30.0"   },
    { OPT_MORSE_SPEED_AUDIO_CHAR,               Setting::TypeFloat,   offsetof( S_config, morse_speed_audio_char ),              "morse.speed.audio.character",            "1",   "60"     },
    { OPT_MORSE_SPEED_AUDIO_EFFECTIVE,          Setting::TypeFloat,   offsetof( S_config, morse_speed_audio_effective ),         "morse.speed.audio.effective",            "1",   "60"     },
    { OPT_MORSE_SPEED_LED_CHAR,                 Setting::TypeFloat,   offsetof( S_config, morse_speed_led_char ),                "morse.speed.led.character",              "1",   "60"     },
    { OPT_MORSE_SPEED_LED_EFFECTIVE,            Setting::TypeFloat,   offsetof( S_config, morse_speed_led_effective ),           "morse.speed.led.effective",              "1",   "60"     },
    { OPT_MORSE_INTERVAL_MULT_CHAR,             Setting::TypeFloat,   offsetof( S_config, morse_interval_multiplier_char ),      "morse.interval_multiplier.character",    "1.0", "10.0"   },
    { OPT_MORSE_INTERVAL_MULT_WORD,             Setting::TypeFloat,   offsetof( S_config, morse_interval_multiplier_word ),      "morse.interval_multiplier.word",         "1.0", "10.0"   },
    { OPT_MORSE_SOUNDER_ARM_DOWN,               Setting::TypeString,  offsetof( S_config, morse_sounder_arm_down ),              "morse.sounder.arm_down",                 "0",   "255"    },
    { OPT_MORSE_SOUNDER_ARM_UP,                 Setting::TypeString,  offsetof( S_config, morse_sounder_arm_up ),                "morse.sounder.arm_up",                   "0",   "255"    },
    { OPT_MORSE_CW_H1_FREQUENCY,                Setting::TypeInt,     offsetof( S_config, morse_cw_h1_frequency ),               "morse.cw.harmonic.h1.frequency",         "100", "5000"   },
    { OPT_MORSE_CW_H1_LEVEL,                    Setting::TypeFloat,   offsetof( S_config, morse_cw_h1_level ),                   "morse.cw.harmonic.h1.level",             "0.0", "10.0"   },
    { OPT_MORSE_CW_H2_MULT,                     Setting::TypeFloat,   offsetof( S_config, morse_cw_h2_multiplier ),              "morse.cw.harmonic.h2.multiplier",        "0.0", "16.0"   },
    { OPT_MORSE_CW_H2_LEVEL,                    Setting::TypeFloat,   offsetof( S_config, morse_cw_h2_level ),                   "morse.cw.harmonic.h2.level",             "0.0", "10.0"   },
    { OPT_MORSE_CW_H3_MULT,                     Setting::TypeFloat,   offsetof( S_config, morse_cw_h3_multiplier ),              "morse.cw.harmonic.h3.multiplier",        "0.0", "16.0"   },
    { OPT_MORSE_CW_H3_LEVEL,                    Setting::TypeFloat,   offsetof( S_config, morse_cw_h3_level ),                   "morse.cw.harmonic.h3.level",             "0.0", "10.0"   },
    { OPT_MORSE_CW_H4_MULT,                     Setting::TypeFloat,   offsetof( S_config, morse_cw_h4_multiplier ),              "morse.cw.harmonic.h4.multiplier",        "0.0", "16.0"   },
    { OPT_MORSE_CW_H4_LEVEL,                    Setting::TypeFloat,   offsetof( S_config, morse_cw_h4_level ),                   "morse.cw.harmonic.h4.level",             "0.0", "10.0"   },
    { OPT_MORSE_CW_RISE_TIME,                   Setting::TypeFloat,   offsetof( S_config, morse_cw_rise_time ),                  "morse.cw.rise_time",                     "1.0", "20.0"   },
    { OPT_MORSE_CW_FALL_TIME,                   Setting::TypeFloat,   offsetof( S_config, morse_cw_fall_time ),                  "morse.cw.fall_time",                     "1.0", "20.0"   },
    { OPT_MORSE_NOISE_BG_FILE,                  Setting::TypeString,  offsetof( S_config, morse_noise_bg_file ),                 "morse.noise.background.file",            "0",   "255"    },
    { OPT_MORSE_NOISE_BG_FADE_TIME,             Setting::TypeFloat,   offsetof( S_config, morse_noise_bg_fade_time ),            "morse.noise.background.fade_time",       "0.0", "10.0"   },
    { OPT_MORSE_NOISE_BG_LEVEL_QUIESCENT,       Setting::TypeFloat,   offsetof( S_config, morse_noise_bg_level_quiescent ),      "morse.noise.background.level.quiescent", "0.0", "10.0"   },
    { OPT_MORSE_NOISE_BG_LEVEL_ACTIVE,          Setting::TypeFloat,   offsetof( S_config, morse_noise_bg_level_active ),         "morse.noise.background.level.active",    "0.0", "10.0"   },
    { OPT_MORSE_NOISE_CW_BURST_START_DURATION,  Setting::TypeFloat,   offsetof( S_config, morse_noise_cw_burst_start_duration ), "morse.noise.cw.burst.start.duration",    "0.0", "5.0"    },
    { OPT_MORSE_NOISE_CW_BURST_START_LEVEL,     Setting::TypeFloat,   offsetof( S_config, morse_noise_cw_burst_start_level ),    "morse.noise.cw.burst.start.level",       "0.0", "10.0"   },
    { OPT_MORSE_NOISE_CW_BURST_END_DURATION,    Setting::TypeFloat,   offsetof( S_config, morse_noise_cw_burst_end_duration ),   "morse.noise.cw.burst.end.duration",      "0.0", "5.0"    },
    { OPT_MORSE_NOISE_CW_BURST_END_LEVEL,       Setting::TypeFloat,   offsetof( S_config, morse_noise_cw_burst_end_level ),      "morse.noise.cw.burst.end.level",         "0.0", "10.0"   }
};

const int options_count = sizeof( options ) / sizeof( options[0] );


C_config::C_config()
{
    // Set default configuration
    config_.display_verbosity                    = 3;
    config_.display_datetime                     = false;
    config_.output_mode                          = "pulseaudio";
    config_.output_file                          = "./morsamdesa.mp3";
    config_.feed_headlines_enabled               = false;
    config_.feed_headlines_delay_min             = 200;
    config_.feed_headlines_delay_min             = 300;
    config_.feed_time_enabled                    = false;
    config_.feed_time_interval                   = 15;
    config_.feed_fixed_enabled                   = true;
    config_.feed_fixed_message                   = "PARIS";
    config_.feed_wordlist_enabled                = false;
    config_.feed_wordlist_random                 = false;
    config_.feed_wordlist_repeat                 = false;
    config_.feed_queue_length                    = 16;
    config_.feed_queue_unheld                    = 1;
    config_.feed_queue_discardable               = 4;
    config_.morse_mode                           = "cw";
    config_.morse_led_enabled                    = false;
    config_.morse_nightmode_start                = "00:00";
    config_.morse_nightmode_end                  = "00:00";
    config_.morse_alphanumeric_only              = false;
    config_.morse_delay_follow_on                = 3.0;
    config_.morse_delay_intermessage             = 3.0;
    config_.morse_speed_audio_char               = 25.0;
    config_.morse_speed_audio_effective          = 12.0;
    config_.morse_speed_led_char                 = 10.0;
    config_.morse_speed_led_effective            = 5.0;
    config_.morse_interval_multiplier_char       = 1.0;
    config_.morse_interval_multiplier_word       = 1.0;
    config_.morse_cw_h1_frequency                = 666;
    config_.morse_cw_h1_level                    = 1.0;
    config_.morse_cw_h2_multiplier               = 2.0;
    config_.morse_cw_h2_level                    = 0.0;
    config_.morse_cw_h3_multiplier               = 3.0;
    config_.morse_cw_h3_level                    = 0.0;
    config_.morse_cw_h4_multiplier               = 4.0;
    config_.morse_cw_h4_level                    = 0.0;
    config_.morse_cw_rise_time                   = 4.0;
    config_.morse_cw_fall_time                   = 4.0;
    config_.morse_noise_bg_level_active          = 0.0;
    config_.morse_noise_bg_level_quiescent       = 0.0;
    config_.morse_noise_cw_burst_start_duration  = 2.0;
    config_.morse_noise_cw_burst_start_level     = 0.0;
    config_.morse_noise_cw_burst_end_duration    = 2.0;
    config_.morse_noise_cw_burst_end_level       = 0.0;
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

    worked = worked && cross_checks();

    if ( worked )
    {
        calculate_derived_settings();
    }

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

    // Parse profile settings
    for ( int option = 0; option < options_count; option++ )
    {
        S_option & op = options[ option ];

        try
        {
            field_path = C_log::format_string( "morsamdesa.profile.%s.%s", profile.c_str(), op.field_path );

            switch ( op.field_type )
            {
                case Setting::TypeString:

                    * ( ( string * ) ( ( ( char * ) &config_ ) + op.field_offset ) ) = cfg_.lookup( field_path.c_str() ).c_str();
                    break;

                case Setting::TypeInt:

                    * ( ( int * ) ( ( ( char * ) &config_ ) + op.field_offset ) ) = cfg_.lookup( field_path.c_str() );
                    break;

                case Setting::TypeFloat:

                    * ( ( float * ) ( ( ( char * ) &config_ ) + op.field_offset ) ) = cfg_.lookup( field_path.c_str() );
                    break;

                case Setting::TypeBoolean:

                    * ( ( bool * ) ( ( ( char * ) &config_ ) + op.field_offset ) ) = cfg_.lookup( field_path.c_str() );
                    break;

                case Setting::TypeArray:
                {
                    const Setting &items_0 = cfg_.lookup( field_path );
                    int   item_count       = items_0.getLength();

                    // Separate out list name element of field path
                    string path;
                    string list;

                    separate_path_and_list( field_path, path, list );

                    const Setting &items = cfg_.lookup( path );

                    for ( int ii = 0; ii < item_count; ii++ )
                    {
                        string str = items[ list.c_str() ][ ii ];

                        ( ( vector< string > & )( * ( ( ( char * ) &config_ ) + op.field_offset ) ) ).push_back( str );
                    }
                    break;
                }

                default:
                    break;
            }
        }
        catch ( const SettingNotFoundException &nfex )
        {
            log_writeln_fmt( C_log::LL_VERBOSE_3, "Missing setting in configuration file: %s", nfex.getPath() );
        }
        catch ( const SettingTypeException &nfex )
        {
            log_writeln_fmt( C_log::LL_ERROR, "Incorrect parameter type in configuration file: %s", nfex.getPath() );
            return false;
        }
        catch ( ... )
        {
            log_writeln( C_log::LL_ERROR, "EXCEPTION" );
        }
    }

    to_enum( config_.output_mode, derived_.output_mode );
    to_enum( config_.morse_mode, derived_.morse_mode );

    // Set logging parameters
    log.initialise( cfg.c().display_verbosity, cfg.c().display_datetime );

    log_writeln_fmt( C_log::LL_INFO, "Applied profile: %s", profile.c_str() );

    return true;
}

bool
C_config::cross_checks()
{
    bool ok = true;

    // Check each setting is within range
    for ( int option = 0; option < options_count; option++ )
    {
        S_option & op = options[ option ];

        try
        {
            switch ( op.field_type )
            {
                case Setting::TypeString:
                {
                    string str = * ( ( string * ) ( ( ( char * ) &config_ ) + op.field_offset ) );
                    
                    if ( ( str.length() < ( size_t ) atoi( op.min ) ) || ( str.length() > ( size_t ) atoi( op.max ) ) )
                    {
                        log_writeln_fmt( C_log::LL_ERROR, "Invalid length: %s - %s", op.field_path, str.c_str() );
                        ok = false;
                    }
                    break;
                }
                case Setting::TypeInt:
                {
                    int i = * ( ( int * ) ( ( ( char * ) &config_ ) + op.field_offset ) );

                    if ( ( i < atoi( op.min ) ) || ( i > atoi( op.max ) ) )
                    {
                        log_writeln_fmt( C_log::LL_ERROR, "Value out of range: %s - %d", op.field_path, i );
                        ok = false;
                    }
                    break;
                }
                case Setting::TypeFloat:
                {
                    float f = * ( ( float * ) ( ( ( char * ) &config_ ) + op.field_offset ) );

                    if ( ( f < atof( op.min ) ) || ( f > atof( op.max ) ) )
                    {
                        log_writeln_fmt( C_log::LL_ERROR, "Value out of range: %s - %f", op.field_path, f );
                        ok = false;
                    }
                    break;
                }
                case Setting::TypeBoolean:
                {
                    break;
                }
                case Setting::TypeArray:
                {
                    vector< string > & array = ( vector< string > & )( * ( ( ( char * ) &config_ ) + op.field_offset ) );

                    for ( unsigned int ii = 0; ii < array.size(); ii++ )
                    {
                        string str = array[ ii ];

                        if ( ( str.length() < ( size_t ) atoi( op.min ) ) || ( str.length() > ( size_t ) atoi( op.max ) ) )
                        {
                            log_writeln_fmt( C_log::LL_ERROR, "Invalid length: %s - %s", op.field_path, str.c_str() );
                            ok = false;
                        }
                    }
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
    if ( config_.feed_headlines_delay_min > config_.feed_headlines_delay_max )
    {
        log_writeln( C_log::LL_ERROR, "feed.headlines.delay.min must be <= than feed_.headlines.delay.max" );
        ok = false;
    }
    if ( config_.feed_headlines_enabled && ( config_.feed_headlines_urls.size() == 0 ) )
    {
        log_writeln( C_log::LL_ERROR, "No headline feed URLs were supplied" );
        ok = false;
    }
    if ( config_.morse_speed_audio_char < config_.morse_speed_audio_effective )
    {
        log_writeln( C_log::LL_ERROR, "morse.speed.audio.character must be >= morse.speed.audio.effective" );
        ok = false;
    }
    if ( config_.morse_speed_led_char < config_.morse_speed_led_effective )
    {
        log_writeln( C_log::LL_ERROR, "morse.speed.led.character must be >= morse.speed.led.effective" );
        ok = false;
    }
    if ( ! ( config_.feed_fixed_enabled || config_.feed_headlines_enabled || config_.feed_time_enabled || config_.feed_wordlist_enabled ) )
    {
        log_writeln( C_log::LL_ERROR, "No data feeds are enabled" );
        ok = false;
    }
    if ( ! valid_time( config_.morse_nightmode_start ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Invalid night mode start time '%s'", config_.morse_nightmode_start.c_str() );
    }
    if ( ! valid_time( config_.morse_nightmode_end ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Invalid night mode end time '%s'", config_.morse_nightmode_end.c_str() );
    }
    if ( config_.feed_queue_length < config_.feed_queue_unheld )
    {
        log_writeln( C_log::LL_ERROR, "feed.queue.length must be >= feed.queue.unheld" );
        ok = false;
    }
    if ( config_.feed_queue_length < config_.feed_queue_discardable )
    {
        log_writeln( C_log::LL_ERROR, "feed.queue.length must be >= feed.queue.discardable" );
        ok = false;
    }

    return ok;
}

void
C_config::calculate_derived_settings()
{
    // With reference to 'A Standard for Morse Timing Using the  Farnsworth Technique.pdf'
    //   http://www.arrl.org/files/file/Technology/x9004008.pdf
    //
    // Using the 'PARIS' 50 unit standard
    //
    //            u = 1.2 / c
    //
    // where u: period of one unit in seconds
    //       c: speed of transmission, in words per minute
    //
    // '+ 5' and '+ 50' - round up to nearest millisecond. All settings are in milliseconds.
    //
    // duration_dot_            = ( ( ( 1000L * 12 ) / wpm ) + 5 ) / 10;
    // duration_dash_           = duration_dot_ * 3;
    // duration_interelement_   = duration_dot_;
    // duration_interletter_    = ( ( ( duration_dot_ * 3 ) * get_multiplier_x100( letter_delay ) ) + 50 ) / 100
    // duration_interword_      = ( ( ( duration_dot_ * 7 ) * get_multiplier_x100( word_delay   ) ) + 50 ) / 100
    //
    // Calculate Farnsworth delay
    //
    // ta is the total delay to add to the characters (31 units) of a standard 50 unit word (PARIS), in seconds
    // ta = ( 60c - 37.2s ) / ( sc )
    // ta = ( ( 60 * char_speed ) - ( 37.2 * effective_speed ) ) / ( char_speed * effective_speed )

    // ------------------------------------------------------------------------------------------------------------
    // Audio duration calculations
    // ------------------------------------------------------------------------------------------------------------

    derived_.audio_duration_element = calculate_element_duration( config_.morse_speed_audio_char );

    calculate_farnsworth_delays( config_.morse_speed_audio_char,   config_.morse_speed_audio_effective
                               , derived_.audio_duration_interletter_fw, derived_.audio_duration_interword_fw );

    derived_.audio_duration_dot          = derived_.audio_duration_element;
    derived_.audio_duration_dash         = derived_.audio_duration_element * 3;
    derived_.audio_duration_interelement = derived_.audio_duration_element;

    // Multiply back up the calculated values to check if they seem plausible
    //
    // Standard PARIS word consists of:
    //   31 elements including inter-element spacing
    //    4 inter-character delays
    //    1 inter-word delay
    //
    // So we should get:
    // ( ( 31 * duration_element_ ) + ( 4 * duration_interletter_ ) + duration_interword_ ) * effective_speed == 60000 (milliseconds)

    derived_.audio_duration_1min_check = calculate_check_1min_duration( derived_.audio_duration_element,
                                                                        derived_.audio_duration_interletter_fw,
                                                                        derived_.audio_duration_interword_fw,
                                                                        config_.morse_speed_audio_effective );

    // Apply any further character and word multipliers
    derived_.audio_duration_interletter  = config_.morse_interval_multiplier_char * derived_.audio_duration_interletter_fw;
    derived_.audio_duration_interword    = config_.morse_interval_multiplier_word * derived_.audio_duration_interword_fw;

    // ------------------------------------------------------------------------------------------------------------
    // LED duration calculations
    // ------------------------------------------------------------------------------------------------------------

    derived_.led_duration_element = calculate_element_duration( config_.morse_speed_led_char );

    calculate_farnsworth_delays( config_.morse_speed_led_char, config_.morse_speed_led_effective,
                                 derived_.led_duration_interletter_fw, derived_.led_duration_interword_fw );

    derived_.led_duration_dot          = derived_.led_duration_element;
    derived_.led_duration_dash         = derived_.led_duration_element * 3;
    derived_.led_duration_interelement = derived_.led_duration_element;

    derived_.led_duration_1min_check = calculate_check_1min_duration( derived_.led_duration_element,
                                                                      derived_.led_duration_interletter_fw,
                                                                      derived_.led_duration_interword_fw,
                                                                      config_.morse_speed_led_effective );

    derived_.led_duration_interletter  = config_.morse_interval_multiplier_char * derived_.led_duration_interletter_fw;
    derived_.led_duration_interword    = config_.morse_interval_multiplier_word * derived_.led_duration_interword_fw;

    // Time used for short delays
    derived_.duration_tick                 = 20;    // mS

    derived_.duration_follow_on            = ( unsigned int ) ( config_.morse_delay_follow_on * 1000.0 );      // mS
    derived_.duration_intermessage         = ( unsigned int ) ( config_.morse_delay_intermessage * 1000.0 );   // mS

    // Convert durations to sample counts. These correspond only to the audio Morse (they're not applicable to LED Morse).

    derived_.dit_samples                   = ( SAMPLE_RATE * derived_.audio_duration_dot ) / 1000;
    derived_.dah_samples                   = derived_.dit_samples * 3;
    
    derived_.interelement_samples          = ( SAMPLE_RATE * derived_.audio_duration_interelement ) / 1000;

    derived_.interletter_samples_fw        = ( SAMPLE_RATE * derived_.audio_duration_interletter_fw ) / 1000;
    derived_.interletter_samples           = ( SAMPLE_RATE * derived_.audio_duration_interletter ) / 1000;
    derived_.interword_samples_fw          = ( SAMPLE_RATE * derived_.audio_duration_interword_fw ) / 1000;
    derived_.interword_samples             = ( SAMPLE_RATE * derived_.audio_duration_interword ) / 1000;

    derived_.intermessage_samples          = SAMPLE_RATE * config_.morse_delay_intermessage;

    derived_.cw_rising_edge_samples        = ( SAMPLE_RATE * config_.morse_cw_rise_time ) / 1000;
    derived_.cw_falling_edge_samples       = ( SAMPLE_RATE * config_.morse_cw_fall_time ) / 1000;
    derived_.cw_burst_noise_start_samples  = ( SAMPLE_RATE * config_.morse_noise_cw_burst_start_duration ) / 1000;
    derived_.cw_burst_noise_end_samples    = ( SAMPLE_RATE * config_.morse_noise_cw_burst_end_duration ) / 1000;
    derived_.bg_noise_fade_samples         = SAMPLE_RATE * config_.morse_noise_bg_fade_time;
    derived_.tick_samples                  = ( SAMPLE_RATE * derived_.duration_tick ) / 1000;

    derived_.cw_h1_level                   = lin_to_exp( config_.morse_cw_h1_level );
    derived_.cw_h2_level                   = lin_to_exp( config_.morse_cw_h2_level );
    derived_.cw_h3_level                   = lin_to_exp( config_.morse_cw_h3_level );
    derived_.cw_h4_level                   = lin_to_exp( config_.morse_cw_h4_level );

    derived_.cw_noise_burst_start_level    = lin_to_exp( config_.morse_noise_cw_burst_start_level );
    derived_.cw_noise_burst_end_level      = lin_to_exp( config_.morse_noise_cw_burst_end_level   );
    derived_.bg_noise_level_active         = config_.morse_noise_bg_level_active / 10.0;
    derived_.bg_noise_level_quiescent      = config_.morse_noise_bg_level_quiescent / 10.0;

    derived_.bg_noise                      = derived_.bg_noise_level_active > 0.0;

    derived_.bg_noise_fading               = ( derived_.bg_noise_level_active > derived_.bg_noise_level_quiescent ) &&
                                             ( config_.morse_noise_bg_fade_time > 0.0 );
}

unsigned int
C_config::calculate_element_duration( float morse_speed_char )
{
    return ( unsigned int ) ( ( ( float ) ( 1000L * 12 ) / morse_speed_char ) + 5 ) / 10;
}

void
C_config::calculate_farnsworth_delays( float        morse_speed_char
                                     , float        morse_speed_effective
                                     , unsigned int & intercharacter_delay
                                     , unsigned int & interword_delay )
{
    // Calculate Farnsworth delays
    //
    // total_delay is the overall delay to add to the characters (31 units) of a standard 50 unit word (PARIS), in seconds
    // total_delay = ( 60c - 37.2s ) / ( sc )
    // total_delay = ( ( 60 * character speed ) - ( 37.2 * effective speed ) ) / ( character speed * effective_speed )
    //
    // then:
    // intercharacter_delay = ( 3 * total_delay ) / 19
    // and:
    // interword_delay      = ( 7 * total_delay ) / 19

    // * 1000 for milliseconds
    unsigned int total_delay = ( unsigned int ) ( ( ( ( 60.0 * morse_speed_char ) - ( ( 372.0 * morse_speed_effective ) / 10.0 ) ) * 1000.0 ) / ( morse_speed_char * morse_speed_effective ) );   // mS

    intercharacter_delay = ( 3 * total_delay ) / 19;       // mS
    interword_delay      = ( 7 * total_delay ) / 19;       // mS
}

unsigned int
C_config::calculate_check_1min_duration( unsigned int duration_element
                                       , unsigned int duration_interletter
                                       , unsigned int duration_interword
                                       , float        effective_char_speed )
{
    return ( unsigned int )( ( float ) ( ( ( 31 * duration_element ) + ( 4 * duration_interletter ) + duration_interword ) ) * effective_char_speed );
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
    log_writeln( C_log::LL_INFO,     "Morsamdesa configuration");
    log_writeln( C_log::LL_INFO,     "------------------------------------------------------------------------------");

    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_DISPLAY_VERBOSITY ),                   config_.display_verbosity                               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_DISPLAY_DATETIME ),                    to_string( config_.display_datetime )                   );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_OUTPUT_MODE ),                         to_string( derived_.output_mode )                       );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_OUTPUT_FILE ),                         config_.output_file.c_str()                             );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_REMOTE_ENABLED ),                      to_string( config_.remote_enabled )                     );

    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_FEED_HEADLINES_ENABLED ),              to_string( config_.feed_headlines_enabled )             );

    unsigned int rss_count = config_.feed_headlines_urls.size();

    for ( unsigned int ii = 0; ii < rss_count; ii++ )
    {
        log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s", to_string( OPT_FEED_HEADLINES_URLS ),                config_.feed_headlines_urls[ ii ].c_str()               );
    }

    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_FEED_HEADLINES_DELAY_MIN ),            config_.feed_headlines_delay_min                        );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_FEED_HEADLINES_DELAY_MAX ),            config_.feed_headlines_delay_max                        );

    unsigned int filter_count = config_.feed_headlines_filters.size();

    for ( unsigned int ii = 0; ii < filter_count; ii++ )
    {
        log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s", to_string( OPT_FEED_HEADLINES_FILTERS ),             config_.feed_headlines_filters[ ii ].c_str()            );
    }

    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_FEED_TIME_ENABLED ),                   to_string( config_.feed_time_enabled )                  );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_FEED_TIME_INTERVAL ),                  config_.feed_time_interval                              );

    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_FEED_FIXED_ENABLED ),                  to_string( config_.feed_fixed_enabled )                 );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_FEED_FIXED_MESSAGE ),                  config_.feed_fixed_message.c_str()                      );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_FEED_QUEUE_LENGTH ),                   config_.feed_queue_length                               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_FEED_QUEUE_UNHELD ),                   config_.feed_queue_unheld                               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_FEED_QUEUE_DISCARDABLE ),              config_.feed_queue_discardable                          );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_FEED_WORDLIST_ENABLED ),               to_string( config_.feed_wordlist_enabled )              );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_FEED_WORDLIST_RANDOM ),                to_string( config_.feed_wordlist_random )               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_FEED_WORDLIST_REPEAT ),                to_string( config_.feed_wordlist_repeat )               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_FEED_WORDLIST_FILE ),                  config_.feed_wordlist_file.c_str()                      );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_MORSE_MODE ),                          to_string( derived_.morse_mode )                        );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_MORSE_LED_ENABLED ),                   to_string( config_.morse_led_enabled )                  );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_MORSE_NIGHTMODE_START ),               config_.morse_nightmode_start.c_str()                   );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_MORSE_NIGHTMODE_END ),                 config_.morse_nightmode_end.c_str()                     );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_MORSE_ALPHANUMERIC_ONLY ),             to_string( config_.morse_alphanumeric_only )            );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_DELAY_FOLLOW_ON ),               config_.morse_delay_follow_on                           );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_DELAY_INTERMESSAGE ),            config_.morse_delay_intermessage                        );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_SPEED_AUDIO_CHAR ),              config_.morse_speed_audio_char                          );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_SPEED_AUDIO_EFFECTIVE ),         config_.morse_speed_audio_effective                     );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_SPEED_LED_CHAR ),                config_.morse_speed_led_char                            );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_SPEED_LED_EFFECTIVE ),           config_.morse_speed_led_effective                       );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_INTERVAL_MULT_CHAR ),            config_.morse_interval_multiplier_char                  );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_INTERVAL_MULT_WORD ),            config_.morse_interval_multiplier_word                  );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_MORSE_SOUNDER_ARM_DOWN ),              config_.morse_sounder_arm_down.c_str()                  );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_MORSE_SOUNDER_ARM_UP ),                config_.morse_sounder_arm_up.c_str()                    );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %d",    to_string( OPT_MORSE_CW_H1_FREQUENCY ),               config_.morse_cw_h1_frequency                           );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_H1_LEVEL ),                   config_.morse_cw_h1_level                               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_H2_MULT ),                    config_.morse_cw_h2_multiplier                          );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_H2_LEVEL ),                   config_.morse_cw_h2_level                               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_H3_MULT ),                    config_.morse_cw_h3_multiplier                          );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_H3_LEVEL ),                   config_.morse_cw_h3_level                               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_H4_MULT ),                    config_.morse_cw_h4_multiplier                          );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_H4_LEVEL ),                   config_.morse_cw_h4_level                               );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_RISE_TIME ),                  config_.morse_cw_rise_time                              );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_CW_FALL_TIME ),                  config_.morse_cw_fall_time                              );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %s",    to_string( OPT_MORSE_NOISE_BG_FILE ),                 config_.morse_noise_bg_file.c_str()                     );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_NOISE_BG_FADE_TIME ),            config_.morse_noise_bg_fade_time                        );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_NOISE_BG_LEVEL_QUIESCENT ),      config_.morse_noise_bg_level_quiescent                  );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_NOISE_BG_LEVEL_ACTIVE ),         config_.morse_noise_bg_level_active                     );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_NOISE_CW_BURST_START_LEVEL ),    config_.morse_noise_cw_burst_start_level                );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_NOISE_CW_BURST_START_DURATION ), config_.morse_noise_cw_burst_start_duration             );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_NOISE_CW_BURST_END_LEVEL ),      config_.morse_noise_cw_burst_end_level                  );
    log_writeln_fmt( C_log::LL_INFO, "%-38.38s: %1.3f", to_string( OPT_MORSE_NOISE_CW_BURST_END_DURATION ),   config_.morse_noise_cw_burst_end_duration               );

    log_writeln( C_log::LL_INFO,     "------------------------------------------------------------------------------" );
    log_writeln( C_log::LL_INFO,     "Derived settings" );
    log_writeln( C_log::LL_INFO,     "------------------------------------------------------------------------------" );

    log_writeln( C_log::LL_INFO, "Audio" );
    log_writeln_fmt( C_log::LL_INFO, "  Dot duration                      : %5u mS",                     derived_.audio_duration_dot               );
    log_writeln_fmt( C_log::LL_INFO, "  Dash duration                     : %5u mS",                     derived_.audio_duration_dash              );
    log_writeln_fmt( C_log::LL_INFO, "  Interelement duration             : %5u mS",                     derived_.audio_duration_interelement      );
    log_writeln_fmt( C_log::LL_INFO, "  Interletter duration (Farnsworth) : %5u mS",                     derived_.audio_duration_interletter_fw    );
    log_writeln_fmt( C_log::LL_INFO, "  Interletter duration (actual)     : %5u mS",                     derived_.audio_duration_interletter       );
    log_writeln_fmt( C_log::LL_INFO, "  Interword duration (Farnsworth)   : %5u mS",                     derived_.audio_duration_interword_fw      );
    log_writeln_fmt( C_log::LL_INFO, "  Interword duration (actual)       : %5u mS",                     derived_.audio_duration_interword         );
    log_writeln_fmt( C_log::LL_INFO, "  Check value                       : %5u mS (should be ~60000 )", derived_.audio_duration_1min_check        );

    log_writeln( C_log::LL_INFO, "LED" );
    log_writeln_fmt( C_log::LL_INFO, "  Dot duration                      : %5u mS",                     derived_.led_duration_dot                 );
    log_writeln_fmt( C_log::LL_INFO, "  Dash duration                     : %5u mS",                     derived_.led_duration_dash                );
    log_writeln_fmt( C_log::LL_INFO, "  Interelement duration             : %5u mS",                     derived_.led_duration_interelement        );
    log_writeln_fmt( C_log::LL_INFO, "  Interletter duration (Farnsworth) : %5u mS",                     derived_.led_duration_interletter_fw      );
    log_writeln_fmt( C_log::LL_INFO, "  Interletter duration (actual)     : %5u mS",                     derived_.led_duration_interletter         );
    log_writeln_fmt( C_log::LL_INFO, "  Interword duration (Farnsworth)   : %5u mS",                     derived_.led_duration_interword_fw        );
    log_writeln_fmt( C_log::LL_INFO, "  Interword duration (actual)       : %5u mS",                     derived_.led_duration_interword           );
    log_writeln_fmt( C_log::LL_INFO, "  Check value                       : %5u mS (should be ~60000 )", derived_.led_duration_1min_check          );

    log_writeln( C_log::LL_INFO, "" );
    log_writeln_fmt( C_log::LL_INFO, "  Tick duration                     : %5u mS",                     derived_.duration_tick                    );
    log_writeln( C_log::LL_INFO, "" );
    log_writeln_fmt( C_log::LL_INFO, "  Follow on duration                : %5u mS",                     derived_.duration_follow_on               );
    log_writeln_fmt( C_log::LL_INFO, "  Intermessage duration             : %5u mS",                     derived_.duration_intermessage            );
    log_writeln( C_log::LL_INFO, "" );

    log_writeln_fmt( C_log::LL_INFO, "  Tick samples                      : %6u",                        derived_.tick_samples                     );
    log_writeln_fmt( C_log::LL_INFO, "  Dit samples                       : %6u",                        derived_.dit_samples                      );
    log_writeln_fmt( C_log::LL_INFO, "  Dah samples                       : %6u",                        derived_.dah_samples                      );
    log_writeln_fmt( C_log::LL_INFO, "  Interelement samples              : %6u",                        derived_.interelement_samples             );
    log_writeln_fmt( C_log::LL_INFO, "  Interletter samples (Farnsworth)  : %6u",                        derived_.interletter_samples_fw           );
    log_writeln_fmt( C_log::LL_INFO, "  Interletter samples               : %6u",                        derived_.interletter_samples              );
    log_writeln_fmt( C_log::LL_INFO, "  Interword samples (Farnsworth)    : %6u",                        derived_.interword_samples_fw             );
    log_writeln_fmt( C_log::LL_INFO, "  Interword samples                 : %6u",                        derived_.interword_samples                );
    log_writeln_fmt( C_log::LL_INFO, "  CW rising edge samples            : %6u",                        derived_.cw_rising_edge_samples           );
    log_writeln_fmt( C_log::LL_INFO, "  CW falling edge samples           : %6u",                        derived_.cw_falling_edge_samples          );
    log_writeln_fmt( C_log::LL_INFO, "  CW burst start samples            : %6u",                        derived_.cw_burst_noise_start_samples     );
    log_writeln_fmt( C_log::LL_INFO, "  CW burst end samples              : %6u",                        derived_.cw_burst_noise_end_samples       );
    log_writeln_fmt( C_log::LL_INFO, "  Background noise fade samples     : %6u",                        derived_.bg_noise_fade_samples            );
    log_writeln( C_log::LL_INFO, "" );

    log_writeln_fmt( C_log::LL_INFO, "  CW harmonic 1 level               : %1.3f",                      derived_.cw_h1_level                      );
    log_writeln_fmt( C_log::LL_INFO, "  CW harmonic 2 level               : %1.3f",                      derived_.cw_h2_level                      );
    log_writeln_fmt( C_log::LL_INFO, "  CW harmonic 3 level               : %1.3f",                      derived_.cw_h3_level                      );
    log_writeln_fmt( C_log::LL_INFO, "  CW harmonic 4 level               : %1.3f",                      derived_.cw_h4_level                      );

    log_writeln_fmt( C_log::LL_INFO, "  CW noise level                    : %1.3f",                      derived_.cw_noise_level                   );
    log_writeln_fmt( C_log::LL_INFO, "  CW noise burst start level        : %1.3f",                      derived_.cw_noise_burst_start_level       );
    log_writeln_fmt( C_log::LL_INFO, "  CW noise burst end level          : %1.3f",                      derived_.cw_noise_burst_end_level         );
    log_writeln_fmt( C_log::LL_INFO, "  CW bg noise active level          : %1.3f",                      derived_.bg_noise_level_active            );
    log_writeln_fmt( C_log::LL_INFO, "  CW bg noise quiescent level       : %1.3f",                      derived_.bg_noise_level_quiescent         );

    log_writeln_fmt( C_log::LL_INFO, "Background noise enabled            : %s",                         to_string( derived_.bg_noise )            );
    log_writeln_fmt( C_log::LL_INFO, "Background noise fading enabled     : %s",                         to_string( derived_.bg_noise_fading )     );

    log_writeln( C_log::LL_INFO,     "------------------------------------------------------------------------------" );
}

const char *
C_config::to_string( bool setting )
{
    return setting ? "true" : "false";
}

const char *
C_config::to_string( eOption opt )
{
    for ( S_option *op = options; op->opt != OPT_INVALID; op++ )
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

    for ( output_mode *p = output_modes; p->output_mode; p++ )
    {
        if ( strncmp( output_mode_s.c_str(), p->output_mode, strlen( p->output_mode ) ) == 0 )
        {
            output_mode_e = p->output_mode_e;
            break;
        }
    }
}

const char *
C_config::to_string( eOutputMode output_mode_e )
{
    for ( output_mode *p = output_modes; p->output_mode; p++ )
    {
        if ( p->output_mode_e  == output_mode_e )
        {
            return p->output_mode;
        }
    }

    return "?";
}

void
C_config::to_enum( const string & morse_mode_s, eMorseMode & morse_mode_e )
{
    morse_mode_e = MM_INVALID;

    for ( morse_mode *p = morse_modes; p->morse_mode; p++ )
    {
        if ( strncmp( morse_mode_s.c_str(), p->morse_mode, strlen( p->morse_mode ) ) == 0 )
        {
            morse_mode_e = p->morse_mode_e;
            break;
        }
    }
}

const char *
C_config::to_string( eMorseMode morse_mode_e )
{
    for ( morse_mode *p = morse_modes; p->morse_mode; p++ )
    {
        if ( p->morse_mode_e  == morse_mode_e )
        {
            return p->morse_mode;
        }
    }

    return "?";
}

/* /brief Convert a linear value in the range 0.0 - 10.0, to an exponential value from 0.0 - 1.0
 *
 */
double
C_config::lin_to_exp( double val )
{
    return pow( val / 10.0, 3 );
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

}
