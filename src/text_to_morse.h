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

// text_to_morse.h

#ifndef text_to_morse_H
#define text_to_morse_H

#include <memory>
#include <string>

#include "morse_enum.h"

using namespace std;

namespace morsamdesa
{

enum eProsign
{
    psNone,
    psStartOfTransmission,
    psTime
};

enum eMorseElement
{
    meNone,
    meDit,
    meDah,
    meDah2,
    meDah3,
    meInterElement,
    meInterElement2,
    meInterCharacter,
    meInterWord,
    meEndOfMessage,
    meInvalid
};

struct S_morse_element_state
{
    eMorseElement morse_elements_[ 1024 ];
    unsigned int  morse_element_count_;
    unsigned int  morse_element_curr_;
};

class C_text_to_morse
{

public:

    C_text_to_morse( bool alphanumeric_only, eMorseCode morse_code );
    ~C_text_to_morse();

    void
    convert( string & message, eProsign prosign );

    string
    get_message();

    eMorseElement
    get_element();

private:

    C_text_to_morse(){}

    void
    prepare_message( const string & message );

    void
    convert_to_morse_elements( char ch );

    const char *
    get_morse_string( char ch );

    void
    add_element( eMorseElement element );

    bool
    ignore( char ch );

    bool
    convert_special_character( const string & message
                             , unsigned int & message_curr
                             , char *       output_buffer
                             , unsigned int & output_buffer_curr );

public:

    S_morse_element_state state_;             // Contains the Morse element sending state

private:

    bool         alphanumeric_only_;
    eMorseCode   morse_code_;

    char         text_[ 1024 ];
    unsigned int text_count_;

    string       text_formatted_;           // The contents of text_ message formatted in a more friendly way for logging (i.e. not uppercase)

    static const char *international_morse_table_[];
    static const char *american_morse_table_[];
    static const char MORSE_CHAR_MAX;
};

}

#endif  // text_to_morse_H
