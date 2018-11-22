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

/*! \file morsamdesa.cpp
    \brief Top-level morsamdesa code
 */

#include "common.h"
#include "config.h"
#include "datafeed.h"
#include "feed_fixed.h"
#include "feed_headlines.h"
#include "feed_time.h"
#include "feed_wordlist.h"
#include "remote_control.h"
#include "keyboard.h"
#include "log.h"
#include "misc.h"
#include "morsamdesa.h"

using namespace  morsamdesa;


namespace morsamdesa
{

extern C_config   cfg;
extern C_log      log;
extern C_keyboard kbd;

const char *VERSION = "0.98";


C_morsamdesa::C_morsamdesa()
{
    rc_ = NULL;
}

C_morsamdesa::~C_morsamdesa()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_morsamdesa destructor" );

    if( rc_ )
    {
        delete rc_;
    }
}

/** \brief main function

    Run using the configured mode

    @param[in]      argc: Number of parameters
    @param[in]      argv: Array of parameter strings
*/
void
C_morsamdesa::run( int argc, char *argv[] )
{
    log_writeln( C_log::LL_INFO, "" );
    log_writeln_fmt( C_log::LL_INFO, "morsamdesa version %s, date %s", VERSION, __DATE__ );
    log_writeln( C_log::LL_INFO, "" );

    if ( cfg.read( argc, argv ) )
    {
        log_writeln( C_log::LL_INFO, "Starting up" );

        rc_ = cfg.c().remote_enabled ? new C_remote_control() : new C_remote_stub();

        if ( main_.initialise() && rc_->initialise() )
        {
            if ( main_.start() && rc_->start() )
            {
                do_datafeeds();

                main_.stop();

                rc_->stop();
            }
        }

        log_writeln( C_log::LL_INFO, "Closed down" );
    }
}

void
C_morsamdesa::do_datafeeds()
{
    C_data_feeds datafeeds;

    log_writeln( C_log::LL_VERBOSE_1, "C_morsamdesa::do_datafeeds()" );

    if ( cfg.c().feed_headlines_enabled )
    {
        for ( unsigned int ii = 0; ii < cfg.c().feed_headlines_urls.size(); ii++ )
        {
            datafeeds.add( new C_feed_headlines( cfg.c().feed_headlines_urls[ ii ],
                                                 cfg.c().feed_headlines_filters,
                                                 cfg.c().feed_headlines_delay_min,
                                                 cfg.c().feed_headlines_delay_max ) );

            log_writeln_fmt( C_log::LL_INFO, "Added headline feed %s", cfg.c().feed_headlines_urls[ ii ].c_str() );
        }
    }

    if ( cfg.c().feed_time_enabled )
    {
        datafeeds.add( new C_feed_time( cfg.c().feed_time_interval ) );

        log_writeln( C_log::LL_INFO, "Added time feed" );
    }

    if ( cfg.c().feed_fixed_enabled )
    {
        datafeeds.add( new C_feed_fixed( &cfg.c().feed_fixed_message ) );

        log_writeln( C_log::LL_INFO, "Added fixed feed" );
    }

    if ( cfg.c().feed_wordlist_enabled )
    {
        datafeeds.add( new C_feed_wordlist( cfg.c().feed_wordlist_file,
                                            cfg.c().feed_wordlist_repeat,
                                            cfg.c().feed_wordlist_random ) );

        log_writeln( C_log::LL_INFO, "Added wordlist feed" );
    }

    datafeeds.start();

    bool all_feeds_done = false;

    while ( ( ! abort() ) && ( ! all_feeds_done ) )
    {
        eCommand cmd = rc_->read();

        if ( cmd != cmdNone )
        {
            main_.command( cmd );
        }

        string  message;
        bool    discard = false;

        if ( datafeeds.read( message, discard, main_.message_queue_full() ) )
        {
            main_.queue_message( message, discard );
        }

        all_feeds_done = datafeeds.all_read();

        delay( 100 );
    }

    datafeeds.stop();

    if ( all_feeds_done )
    {
        log_writeln( C_log::LL_INFO, "Data feeds are finished" );

        main_.wait_all_sent();
    }

    log_writeln( C_log::LL_INFO, "Closing down" );
}

bool
C_morsamdesa::abort()
{
    int ch = C_keyboard::NO_KEY;

    if ( kbd.got_keypress() )
    {
        ch = kbd.get_key();
    }

    return ( ch == C_keyboard::ABORT_KEY_1 ) || ( ch == C_keyboard::ABORT_KEY_2 );
}

}

/** \brief main function

    Run using the configured mode

    @param[in]      argc: Number of parameters
    @param[in]      argv: Array of parameter strings
*/
int main( int argc, char *argv[] )
{
    try
    {
        C_morsamdesa morsamdesa;

        morsamdesa.run( argc, argv );
    }
    catch ( exception & ex )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Program exception: %s", ex.what() );
    }
    
    catch ( ... )
    {
        log_writeln( C_log::LL_ERROR, "Program exception" );
    }

    return 0;
}
