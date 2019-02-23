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

// main_proc.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "audio_morse_sparkgap.h"
#include "audio_output_mp3.h"
#include "audio_output_pulseaudio.h"
#include "config.h"
#include "log.h"
#include "main_proc.h"
#include "misc.h"
#include "timer.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config     cfg;
extern C_log        log;

C_main_proc::C_main_proc()
{
    main_state_         = C_wait_for_message::s.instance();
    command_state_      = C_wait_for_command::s.instance();
    command_            = cmdNone;

    follow_on_          = false;
    interrupt_          = false;
    muted_              = false;
    prefix_             = cfg.c().feed_headlines_prefix;
    mute_request_       = false;
    mute_in_progress_   = false;
    send_in_progress_   = false;
    play_last_message_  = false;
    play_next_message_  = false;
    close_down_         = false;
}

C_main_proc::~C_main_proc()
{
}

void
C_main_proc::change_state_to( shared_ptr< C_main_state > state )
{
    main_state_ = state;
}

void
C_main_proc::change_state_to( shared_ptr< C_command_state > state )
{
    command_state_ = state;
}

bool
C_main_proc::initialise()
{
    message_queue_.initialise( cfg.c().feed_queue_unheld );

    if ( cfg.c().output_mode == OMODE_PULSEAUDIO )
    {
        audio_output_.reset( new C_audio_output_pulseaudio() );
    }
    else
    {
        audio_output_.reset( new C_audio_output_mp3() );
    }

    transmitter_.reset( new C_transmitter() );

    bool worked = transmitter_->initialise( cfg.c().transmitters, audio_output_ );

    command_sounds_.reset( new C_audio_command( transmitter_->find( "CMD" ) ) );
    led_morse_.reset( cfg.c().output_led ? new C_led_morse( transmitter_->find( "LED" ) ) : new C_morse() );
    background_noise_.reset( new C_noise_file( cfg.c().noise_bg_file, smLoop ) );

    worked = worked && audio_output_->initialise();
    worked = worked && background_noise_->initialise( audio_output_ );
    worked = worked && command_sounds_->initialise( audio_output_ );
    worked = worked && intermessage_.initialise( cfg.d().intermessage_samples, audio_output_ );
    worked = worked && led_morse_->initialise();

    night_mode_.initialise( cfg.c().nightmode_start, cfg.c().nightmode_end );

    if ( worked )
    {
        // Set initial mute state
        muted_ = night_mode_.active();
        led_morse_->muted( muted_ );

        log_writeln_fmt( C_log::LL_INFO, "Night mode %s", muted_ ? "on" : "off" );
        log_writeln_fmt( C_log::LL_INFO, "LED output %s", cfg.c().output_led ? "enabled": "disabled" );

        if ( cfg.c().transmitters[ 0 ].alphanumeric_only )
        {
            log_writeln( C_log::LL_INFO, "Alphanumeric characters only" );
        }
    }

    return worked;
}

bool
C_main_proc::start()
{
    return led_morse_->start() && thread_start();
}

void
C_main_proc::stop()
{
    close_down_ = true;
    
    led_morse_->stop();

    thread_await_exit();
}

void
C_main_proc::command( eCommand cmd )
{
    command_ = cmd;
}

void
C_main_proc::queue_message( C_data_feed_entry & feed_item )
{
    message_queue_.add( feed_item );
}

void
C_main_proc::wait_all_sent()
{
    C_timer timer;

    // Fixed delay of one second to allow enough time for a message be read from the queue and sent for transmission
    delay( 1000 );

    log_writeln( C_log::LL_INFO, "Waiting for outstanding messages to be sent" );

    while ( true )
    {
        if ( send_in_progress_ )
        {
			// Allow enough time for background noise to fade down, if configured
            timer.start( 2000 + ( long ) ( ( cfg.c().noise_bg_fade_time > 0.0 ) ? ( 1000.0 * cfg.c().noise_bg_fade_time ) : 0.0 ) );
        }

        if ( timer.expired() )
        {
            break;
        }
        
        delay( 100 );
    }

    log_writeln( C_log::LL_INFO, "All sent" );
}

// -----------------------------------------------------------------------------------
// Background thread code
// -----------------------------------------------------------------------------------

void
C_main_proc::thread_handler()
{
    // This thread handles the main loop for:
    // - Audio output, supplying a constant steam of samples to Pulseaudio, whether or not any Morse/background noise information is present
    // - Main process flow for Morse message output
    // - Response to input commands
    //
    // LED Morse output is initiated from the main state code and implemented in the C_led_morse background thread

    while ( ! close_down_ )
    {
        command_state_->handler( this );            // Command states are implemented in command_state_.cpp
        main_state_->handler( this );               // Message states are implemented in main_state_.cpp

        audio_output_->clear_buffer();              // Initialise output buffer

        // Fetch one tick's worth (20mS) of samples from each of the audio feeds and add them into the audio output.

        transmitter_->write();                      // Add to output buffer
        background_noise_->write();                 //
        intermessage_.write();                      //
        command_sounds_->write();                   //

        if ( muted_ )                               // If muted, clear down the buffer we have just been adding to.
        {                                           // It's a rather wasteful approach, but it allows the sample feeds to
            audio_output_->clear_buffer();          // carry on as normal, rather than pausing the sounds - so sound
        }                                           // will be lost as would be expected from a mute.

        audio_output_->send();                      // Output audio
    }

    audio_output_->send( true );                    // Make sure all audio output is flushed
    
    message_queue_.discard_all_messages();
}

bool
C_main_proc::message_waiting()
{
    return play_last_message_ || play_next_message_ || message_queue_.got_unplayed_message();
}

C_data_feed_entry
C_main_proc::read_message()
{
    C_data_feed_entry feed_entry;

    if ( play_last_message_ )
    {
        message_queue_.get_most_recent( feed_entry );
        play_last_message_ = false;
    }
    else if ( play_next_message_ )
    {
        message_queue_.get_next_held( feed_entry );
        play_next_message_ = false;
    }
    else
    {
        message_queue_.get_next_unplayed( feed_entry );
    }

    return feed_entry;
}

void
C_main_proc::mark_message_read( C_data_feed_entry & feed_entry )
{
    message_queue_.mark_as_played( feed_entry );
}

}
