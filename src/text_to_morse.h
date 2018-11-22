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

// text_to_morse.h

#ifndef text_to_morse_H
#define text_to_morse_H

#include <string>

using namespace std;

namespace morsamdesa
{

const int STX = 0x02;

enum eMorseElement
{
    meNone,
    meDit,
    meDah,
    meInterElement,
    meInterCharacter,
    meInterWord,
    meEndOfMessage,
    meInvalid
};

class C_text_to_morse
{

public:

    C_text_to_morse();
    ~C_text_to_morse();

    void
    convert( string & message, bool add_stx );

    const char *
    get_message();

    eMorseElement
    get_element();

private:

    void
    prepare_message( string & message, bool add_stx );

    void
    convert_to_morse( char ch );

    void
    add_element( eMorseElement element );

    bool
    ignore( char ch );

    bool
    convert_special_character( const string & message, unsigned int & message_curr,
                               char *output_buffer,    unsigned int & output_buffer_curr );

private:

    char         text_[ 1024 ];
    unsigned int text_count_;

    string       text_formatted_;           // The contents of text_ message formatted in a more friendly way for logging (i.e. not uppercase)

    eMorseElement morse_elements_[ 1024 ];
    unsigned int  morse_element_count_;
    unsigned int  morse_element_curr_;

    static const char *morse_table_[];
    static const char MORSE_CHAR_MAX;
};

}

#endif  // text_to_morse_H
