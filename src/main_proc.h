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

// main_proc.h

#ifndef main_proc_H
#define main_proc_H

#include <memory>

#include "audio_command.h"
#include "audio_morse_cw.h"
#include "audio_morse_double_plate_sounder.h"
#include "audio_morse_sounder.h"
#include "audio_output.h"
#include "command.h"
#include "command_state.h"
#include "datafeed.h"
#include "led_morse.h"
#include "message_queue.h"
#include "main_state.h"
#include "night_mode.h"
#include "noise_file.h"
#include "silence.h"
#include "sound_file.h"
#include "thread.h"
#include "timer.h"
#include "transmitter.h"

using namespace std;

namespace morsamdesa
{

#define LED_MORSE_ACTIVE ( cfg.c().output_led && p->muted_ )

const long FOLLOW_ON_TIME    = 1500;    // mS
const long INTERMESSAGE_TIME = 3000;    // mS

enum eSound
{
    sStartTone,
    sIntermediateTone,
    sEndTone
};


class C_main_proc : public C_thread
{
    friend class C_main_state;
    friend class C_wait_for_message;
    friend class C_quiescent;
    friend class C_preamble;
    friend class C_preamble_wait;
    friend class C_message_send;
    friend class C_message_send_wait;
    friend class C_message_end;
    friend class C_follow_on_wait;
    friend class C_inter_message;
    friend class C_inter_message_wait;
    friend class C_postamble;
    friend class C_postamble_wait;

    friend class C_command_state;
    friend class C_wait_for_command;
    friend class C_check_night_mode;
    friend class C_mute;
    friend class C_unmute;
    friend class C_toggle_mute;
    friend class C_mute_unmute_wait;
    friend class C_previous_message;
    friend class C_previous_message_wait;
    friend class C_next_message;
    friend class C_next_message_wait;
    friend class C_interrupt_message;
    friend class C_interrupt_message_wait;
    friend class C_toggle_prefix;
    friend class C_toggle_prefix_wait;
         
public:

    C_main_proc();
    virtual ~C_main_proc();

    bool
    initialise();

    bool
    start();

    void
    stop();

    void
    queue_message( C_data_feed_entry & feed_item );

    void
    wait_all_sent();

    void
    command( eCommand cmd  );

private:

    // Worker thread
    void
    change_state_to( shared_ptr< C_main_state > state  );

    void
    change_state_to( shared_ptr< C_command_state > state );

    void
    thread_handler();

    bool
    message_waiting();

    C_data_feed_entry
    read_message();

    void
    mark_message_read( C_data_feed_entry & feed_entry );

private:

    shared_ptr< C_main_state >     main_state_;
    shared_ptr< C_command_state >  command_state_;
    shared_ptr< C_audio_output >   audio_output_;

    unique_ptr< C_transmitter >    transmitter_;
    unique_ptr< C_morse >          led_morse_;
    unique_ptr< C_noise_file >     background_noise_;

    unique_ptr< C_audio_command >  command_sounds_;

    C_message_queue     message_queue_;
    C_data_feed_entry   feed_item_;

    C_silence           intermessage_;

    C_timer             follow_on_timer_;
    C_night_mode        night_mode_;

    eCommand            command_;

    bool                follow_on_;
    bool                interrupt_;
    bool                muted_;
    bool                prefix_;
    bool                mute_request_;
    bool                mute_in_progress_;
    bool                send_in_progress_;
    bool                play_last_message_;
    bool                play_next_message_;
    bool                close_down_;
};

}

#endif // main_proc_H
