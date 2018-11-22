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

// main_proc.cpp

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

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
    message_queue_      = NULL;
    audio_morse_        = NULL;
    led_morse_          = NULL;
    background_noise_   = NULL;
    audio_output_       = NULL;

    follow_on_          = false;
    interrupt_          = false;
    muted_              = false;
    mute_request_       = false;
    mute_in_progress_   = false;
    send_in_progress_   = false;
    play_last_message_  = false;
    play_next_message_  = false;
    close_down_         = false;
}

C_main_proc::~C_main_proc()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_main_proc destructor" );

    if ( main_state_ )
    {
        delete main_state_;
    }
    if ( command_state_ )
    {
        delete command_state_;
    }
    if ( audio_morse_ )
    {
        delete audio_morse_;
    }
    if ( background_noise_ )
    {
        delete background_noise_;
    }
    if ( audio_output_ )
    {
        delete audio_output_;
    }
    if ( led_morse_ )
    {
        delete led_morse_;
    }
    if ( message_queue_ )
    {
        delete message_queue_;
    }
}

void
C_main_proc::change_state_to( C_main_state * state )
{
    main_state_ = state;
}

void
C_main_proc::change_state_to( C_command_state * state )
{
    command_state_ = state;
}

bool
C_main_proc::initialise()
{
    if ( cfg.d().output_mode == OMODE_PULSEAUDIO )
    {
        audio_output_ =  new C_audio_output_pulseaudio();
    }
    else
    {
        audio_output_ =  new C_audio_output_mp3();
    }

    background_noise_ = new C_noise_file( cfg.c().morse_noise_bg_file, smLoop );

    if ( cfg.d().morse_mode == MM_SOUNDER )
    {
        audio_morse_ = new C_audio_morse_sounder( text_to_morse_ );
    }
    else
    {
        audio_morse_ = new C_audio_morse_cw( text_to_morse_ );
    }

    led_morse_ = cfg.c().morse_led_enabled ? new C_led_morse( text_to_morse_ ) : new C_morse( text_to_morse_ );

    message_queue_ = new C_message_queue( cfg.c().feed_queue_length, cfg.c().feed_queue_unheld, cfg.c().feed_queue_discardable );

    bool worked = audio_output_->initialise();

    worked = worked && audio_morse_->initialise( audio_output_ );
    worked = worked && background_noise_->initialise( audio_output_ );
    worked = worked && command_sounds_.initialise( audio_output_ );
    worked = worked && intermessage_.initialise( cfg.d().intermessage_samples, audio_output_ );

    worked = worked && led_morse_->initialise();

    night_mode_.initialise( cfg.c().morse_nightmode_start, cfg.c().morse_nightmode_end );

    if ( worked )
    {
        // Set initial mute state
        muted_ = night_mode_.active();
        led_morse_->muted( muted_ );

        log_writeln_fmt( C_log::LL_INFO, "Night mode %s", muted_ ? "on" : "off" );

        log_writeln_fmt( C_log::LL_INFO, "LED output %s", cfg.c().morse_led_enabled ? "enabled": "disabled" );

        if ( cfg.c().morse_alphanumeric_only )
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
C_main_proc::queue_message( string & message, bool discard )
{
    message_queue_->add( message, discard );
}

bool
C_main_proc::message_queue_full()
{
    return message_queue_->full();
}

void
C_main_proc::wait_all_sent()
{
    // Wait until the queue being empty of unplayed message, and no message being sent,
    // is true for two seconds.
    C_timer timer;

    log_writeln( C_log::LL_INFO, "Waiting for outstanding messages to be sent" );

    while ( true )
    {
        if ( ( ! message_queue_->empty() ) || send_in_progress_ )
        {
            timer.start( cfg.c().morse_noise_bg_fade_time );
        }

        if ( timer.expired() )
        {
            break;
        }
        
        log_writeln( C_log::LL_INFO, "Waiting..." );

        delay( 100 );
    }
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
    // - Response to remote control commands
    //
    // LED Morse output is initiated from the main state code, and implemented in the C_led_morse background thread

    while ( ! close_down_ )
    {
        command_state_->handler( this );            // Command states are implemented in command_state_.cpp
        main_state_->handler( this );               // Message states are implemented in main_state_.cpp

        audio_output_->clear_buffer();              // Initialise output buffer

        // Fetch one tick's worth (20mS) of samples from each of the audio feeds and add them into the audio output.

        audio_morse_->write();                      // Add to output buffer
        background_noise_->write();                 //
        intermessage_.write();                      //
        command_sounds_.write();                    //

        if ( muted_ )                               // If muted, clear down the buffer we have just been adding to.
        {                                           // It's a rather wasteful approach, but it allows the sample feeds to
            audio_output_->clear_buffer();          // carry on as normal, rather than pausing the sounds - so sound
        }                                           // will be lost as would be expected from a mute.

        audio_output_->send();                      // Output audio
    }

    audio_output_->send( true );                    // Make sure all audio output is flushed
    
    message_queue_->discard_all_messages();
}

bool
C_main_proc::message_waiting()
{
    return play_last_message_ || play_next_message_ || message_queue_->got_unplayed_message();
}

string
C_main_proc::read_message()
{
    string msg;

    if ( play_last_message_ )
    {
        message_queue_->get_most_recent( msg );
        play_last_message_ = false;
    }
    else if ( play_next_message_ )
    {
        message_queue_->get_next_held( msg );
        play_next_message_ = false;
    }
    else
    {
        message_queue_->get_next_unplayed( msg );
    }

    return msg;
}

void
C_main_proc::mark_message_read( string & msg )
{
    message_queue_->mark_as_played( msg );
}

}
