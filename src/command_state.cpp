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

// command_state.cpp

#include "common.h"
#include "log.h"
#include "misc.h"
#include "timer.h"

#include "command.h"
#include "command_state.h"
#include "main_proc.h"
#include "night_mode.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;

// ------------------------------------------------------------------------------------------------
// C_command_state
// ------------------------------------------------------------------------------------------------

void
C_command_state::change_state_to( C_main_proc * morse, shared_ptr< C_command_state > state, const char * description )
{
    log_writeln_fmt( C_log::LL_VERBOSE_3, "CMD  state: %s", description );

    morse->change_state_to( state );
}

void
C_command_state::handler( C_main_proc * p )
{
    log_writeln( C_log::LL_VERBOSE_3, "CMD  C_command_state::handler()" );
}

// ------------------------------------------------------------------------------------------------
// C_wait_for_command
// ------------------------------------------------------------------------------------------------

void
C_wait_for_command::handler( C_main_proc * p )
{
    switch ( p->command_ )
    {
        case cmdStop:
            change_state_to( p, C_interrupt_message::s.instance(), "C_interrupt_message" );
            break;

        case cmdPrevious:
            change_state_to( p, C_previous_message::s.instance(), "C_previous_message" );
            break;

        case cmdMute:
            change_state_to( p, C_toggle_mute::s.instance(), "C_toggle_mute" );
            break;
        
        case cmdNext:
            change_state_to( p, C_next_message::s.instance(), "C_next_message" );
            break;
        
        case cmdPrefix:
            change_state_to( p, C_toggle_prefix::s.instance(), "C_toggle_prefix" );
            break;
        
        default:
            change_state_to( p, C_check_night_mode::s.instance(), "C_check_night_mode" );
            break;
    }
    
    p->command_ = cmdNone;     // We've acted upon command, so reset it
}

// ------------------------------------------------------------------------------------------------
// C_check_night_mode
// ------------------------------------------------------------------------------------------------

void
C_check_night_mode::handler( C_main_proc * p )
{
    static unsigned int delay_count = 0;

    // Check night mode once a second

    if ( ( delay_count++ % 50 ) == 0 )
    {
        switch ( p->night_mode_.status() )
        {
            case nmEntered:
                log_writeln( C_log::LL_INFO, "Night mode on" );
                change_state_to( p, C_mute::s.instance(), "C_mute" );
                break;

            case nmLeft:
                log_writeln( C_log::LL_INFO, "Night mode off" );
                change_state_to( p, C_unmute::s.instance(), "C_unmute" );
                break;
            default:    
                change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
                break;
        }
    }
    else
    {
        delay( 20 );
        change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_mute
// ------------------------------------------------------------------------------------------------

void
C_mute::handler( C_main_proc * p )
{
    if ( ! p->muted_ )
    {
        change_state_to( p, C_toggle_mute::s.instance(), "C_toggle_mute");
    }
    else
    {
        change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_unmute
// ------------------------------------------------------------------------------------------------

void
C_unmute::handler( C_main_proc * p )
{
    if ( p->muted_ )
    {
        change_state_to( p, C_toggle_mute::s.instance(), "C_toggle_mute");
    }
    else
    {
        change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_toggle_mute
// ------------------------------------------------------------------------------------------------

void
C_toggle_mute::handler( C_main_proc * p )
{
    if ( p->muted_ )
    {
        p->muted_ = false;
        p->led_morse_->muted( false );
        p->command_sounds_->trigger( ctUnmute );

        if ( cfg.c().output_led && p->led_morse_->busy() )
        {
            p->led_morse_->interrupt();
            p->transmitter_->set_send_state( p->led_morse_->get_send_state() );
            p->transmitter_->resume_sending();
        }
    }
    else
    {
        p->command_sounds_->trigger( ctMute );
        p->mute_in_progress_ = true;
    }

    change_state_to( p, C_mute_unmute_wait::s.instance(), "C_mute_unmute_wait ");
}

// ------------------------------------------------------------------------------------------------
// C_mute_unmute_wait
// ------------------------------------------------------------------------------------------------

void
C_mute_unmute_wait::handler( C_main_proc * p )
{
    if ( ! p->command_sounds_->busy() )
    {
        if ( ! p->muted_ && p->mute_in_progress_ )
        {
            // We're done: if we were not muted, then, having played the mute notification sound, it's now ok to mute the output
            p->muted_            = true;
            p->mute_in_progress_ = false;

            p->led_morse_->muted( true );

            if ( cfg.c().output_led && p->transmitter_->busy() )
            {
                p->transmitter_->interrupt();
                p->led_morse_->set_send_state( p->transmitter_->get_send_state() );
                p->led_morse_->resume_sending();
            }
        }

        change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_previous_message
// ------------------------------------------------------------------------------------------------

void
C_previous_message::handler( C_main_proc * p )
{
    p->command_sounds_->trigger( ctPrevious );

    change_state_to( p, C_previous_message_wait::s.instance(), "C_previous_message_wait" );
}

// ------------------------------------------------------------------------------------------------
// C_previous_message_wait
// ------------------------------------------------------------------------------------------------

void
C_previous_message_wait::handler( C_main_proc * p )
{
    if ( ! p->command_sounds_->busy() )
    {
        p->play_last_message_ = true;

        if ( p->send_in_progress_ )
        {
            p->interrupt_ = true;
        }

        change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_next_message
// ------------------------------------------------------------------------------------------------

void
C_next_message::handler( C_main_proc * p )
{
    p->command_sounds_->trigger( ctNext );

    change_state_to( p, C_next_message_wait::s.instance(), "C_next_message_wait" );
}

// ------------------------------------------------------------------------------------------------
// C_next_message_wait
// ------------------------------------------------------------------------------------------------

void
C_next_message_wait::handler( C_main_proc * p )
{
    if ( ! p->command_sounds_->busy() )
    {
        p->play_next_message_ = true;

        if ( p->send_in_progress_ )
        {
            p->interrupt_ = true;
        }

        change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_interrupt_message
// ------------------------------------------------------------------------------------------------

void
C_interrupt_message::handler( C_main_proc * p )
{
    p->command_sounds_->trigger( ctInterrupt );

    change_state_to( p, C_interrupt_message_wait::s.instance(), "C_interrupt_message_wait" );
}

// ------------------------------------------------------------------------------------------------
// C_interrupt_message_wait
// ------------------------------------------------------------------------------------------------

void
C_interrupt_message_wait::handler( C_main_proc * p )
{
    if ( ! p->command_sounds_->busy() )
    {
        if ( p->send_in_progress_ )
        {
            p->interrupt_ = true;
        }

        change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
    }
}

// ------------------------------------------------------------------------------------------------
// C_toggle_prefix
// ------------------------------------------------------------------------------------------------

void
C_toggle_prefix::handler( C_main_proc * p )
{
    p->command_sounds_->trigger( ctPrefix );

    change_state_to( p, C_toggle_prefix_wait::s.instance(), "C_toggle_prefix_wait" );
}

// ------------------------------------------------------------------------------------------------
// C_toggle_prefix_wait
// ------------------------------------------------------------------------------------------------
void
C_toggle_prefix_wait::handler( C_main_proc * p )
{
    if ( ! p->command_sounds_->busy() )
    {
        p->prefix_ = ! p->prefix_;
        
        log_writeln_fmt( C_log::LL_INFO, "Prefix %s", p->prefix_ ? "on" : "off" );

        change_state_to( p, C_wait_for_command::s.instance(), "C_wait_for_command" );
    }
}

}
