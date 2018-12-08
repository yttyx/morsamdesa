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

// main_state.cpp

#include "common.h"
#include "config.h"
#include "log.h"
#include "main_proc.h"
#include "main_state.h"
#include "misc.h"
#include "timer.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;

// ------------------------------------------------------------------------------------------------
// C_main_state
// ------------------------------------------------------------------------------------------------

void
C_main_state::change_state_to( C_main_proc * main_proc, C_main_state * main_state, const char * description )
{
    log_writeln_fmt( C_log::LL_VERBOSE_2, "MAIN state: %s", description );

    main_proc->change_state_to( main_state );
}

void
C_main_state::handler( C_main_proc * p )
{
    log_writeln( C_log::LL_VERBOSE_2, "MAIN C_main_state::handler()" );
}

// ------------------------------------------------------------------------------------------------
// C_wait_for_message
// ------------------------------------------------------------------------------------------------

void
C_wait_for_message::handler( C_main_proc * p )
{
    static bool once = false;

    if ( ! once )
    {
        log_writeln( C_log::LL_VERBOSE_1, "Waiting for message" );
        once = true;
    }

    if ( p->message_waiting() )
    {
        if ( p->follow_on_ )
        {
            change_state_to( p, C_message_send::s.instance(), "C_message_send" );
            once = false;
        }
        else
        {
            change_state_to( p, C_preamble::s.instance(), "C_preamble" );
            once = false;
        }
    }
}

// ------------------------------------------------------------------------------------------------
// C_preamble
// ------------------------------------------------------------------------------------------------

void
C_preamble::handler( C_main_proc * p )
{
    if ( cfg.d().bg_noise && cfg.d().bg_noise_fading )
    {
        p->background_noise_->trigger( nsFadeUp );

        change_state_to( p, C_preamble_wait::s.instance(), "C_preamble_wait" );
    }
    else
    {
        change_state_to( p, C_message_send::s.instance(), "C_message_send" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_preamble_wait
// ------------------------------------------------------------------------------------------------

void
C_preamble_wait::handler( C_main_proc * p )
{
    if ( p->background_noise_->state() == nsUnchanging )
    {
        change_state_to( p, C_message_send::s.instance(), "C_message_send" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_message_send
// ------------------------------------------------------------------------------------------------

void
C_message_send::handler( C_main_proc * p )
{
    // Precede the message with an STX if the follow on timer from the last message timed out
    bool add_stx = ! p->follow_on_;

    p->message_ = p->read_message();
    p->text_to_morse_.convert( p->message_, add_stx );

    if ( LED_MORSE_ACTIVE )
    {
        p->led_morse_->start_sending();
    }
    else
    {
        p->audio_morse_->start_sending();
    }
    
    p->send_in_progress_ = true;

    log_writeln_fmt( C_log::LL_INFO, "%s", p->text_to_morse_.get_message().c_str() );

    change_state_to( p, C_message_send_wait::s.instance(), "C_message_send_wait" );
}

// ------------------------------------------------------------------------------------------------
// C_message_send_wait
// ------------------------------------------------------------------------------------------------

void
C_message_send_wait::handler( C_main_proc * p )
{
    if ( p->interrupt_ )
    {
        p->audio_morse_->interrupt();
        p->led_morse_->interrupt();

        change_state_to( p, C_message_end::s.instance(), "C_message_end" );
    }
    
    bool busy = p->led_morse_->busy() || p->audio_morse_->busy();

    if ( ! busy )
    {
        change_state_to( p, C_message_end::s.instance(), "C_message_end" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_message_end
// ------------------------------------------------------------------------------------------------
// We can get here either because the message has been sent, or if an interrupt message command
// was received.
void
C_message_end::handler( C_main_proc * p )
{
    p->interrupt_        = false;
    p->send_in_progress_ = false;

    p->mark_message_read( p->message_ );

    p->follow_on_ = true;
    p->follow_on_timer_.start( cfg.d().duration_follow_on );

    change_state_to( p, C_follow_on_wait::s.instance(), "C_follow_on_wait" );
}

// ------------------------------------------------------------------------------------------------
// C_follow_on_wait
// ------------------------------------------------------------------------------------------------

void
C_follow_on_wait::handler( C_main_proc * p )
{
    if ( p->message_waiting() )
    {
        change_state_to( p, C_inter_message::s.instance(), "C_inter_message" );
    }
    else
    {
        if ( p->follow_on_timer_.expired() )
        {
            p->follow_on_ = false;

            change_state_to( p, C_postamble::s.instance(), "C_postamble" );
        }
    }
}

// ------------------------------------------------------------------------------------------------
// C_inter_message
// ------------------------------------------------------------------------------------------------

void
C_inter_message::handler( C_main_proc * p )
{
    p->intermessage_.trigger();

    change_state_to( p, C_inter_message_wait::s.instance(), "C_inter_message_wait" );
}

// ------------------------------------------------------------------------------------------------
// C_inter_message_wait
// ------------------------------------------------------------------------------------------------

void
C_inter_message_wait::handler( C_main_proc * p )
{
    if ( ! p->intermessage_.active() )
    {
        change_state_to( p, C_message_send::s.instance(), "C_message_send" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_postamble
// ------------------------------------------------------------------------------------------------

void
C_postamble::handler( C_main_proc * p )
{
    if ( cfg.d().bg_noise && cfg.d().bg_noise_fading )
    {
        p->background_noise_->trigger( nsFadeDown );

        change_state_to( p, C_postamble_wait::s.instance(), "C_postamble_wait" );
    }
    else
    {
        change_state_to( p, C_wait_for_message::s.instance(), "C_wait_for_message" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_postamble_wait
// ------------------------------------------------------------------------------------------------

void
C_postamble_wait::handler( C_main_proc * p )
{
    if ( p->background_noise_->state() == nsUnchanging )
    {
        change_state_to( p, C_wait_for_message::s.instance(), "C_wait_for_message" );
    }
}

}
