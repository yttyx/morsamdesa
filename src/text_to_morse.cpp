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

// text_to_morse.cpp

#include <assert.h>
#include <string>
#include <string.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "text_to_morse.h"


using namespace  morsamdesa;

namespace morsamdesa
{

extern C_config cfg;
extern C_log    log;



struct special_character_entry
{
    const char  *from;
    const char  *to;
};

static special_character_entry special_character_table[] =
{
    { "A$",             "AUD"       },
    { "$",              "USD"       },
    { "&",              "+"         },
    { "\xA3",           "GBP"       },
    { "\xC2\xA3",       "GBP"       },
    { "\xE2\x82\xAC",   "EUR"       },
    { NULL,             NULL        }
};

C_text_to_morse::C_text_to_morse()
{
    morse_element_count_ = 0;
    morse_element_curr_  = 0;
    text_count_          = 0;
}

C_text_to_morse::~C_text_to_morse()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_text_to_morse destructor" );
}

void
C_text_to_morse::convert( string & message, bool add_stx )
{
    // Format message into text_
    prepare_message( message, add_stx );

    log_writeln_fmt( C_log::LL_VERBOSE_2, "C_text_to_morse::convert() (after prepare) : %s", text_ );
    
    morse_element_count_ = 0;
    morse_element_curr_  = 0;

    // For each character in the text
    for ( unsigned int text_curr = 0; ( text_curr < text_count_ ) && ( morse_element_count_ < ( NUMBEROF( morse_elements_ ) - 3 ) ); text_curr++ )
    {
        // Convert to Morse elements and add the elements onto a list for later retrieval
        char ch_curr = toupper( text_[ text_curr ] );
        char ch_next = text_[ text_curr + 1 ];

        if ( isspace( ch_curr ) )
        {
            add_element( meInterWord );
        }
        else
        {
            convert_to_morse( ch_curr );

            if ( ch_next && ( ! isspace( ch_next ) ) )
            {
                add_element( meInterCharacter );
            }
        }
    }
    
    // The final meInterWord is for the benefit of the sounder output. The armature-up sound is contained
    // in the interelement, interletter and interword samples, so we require a dummy meInterWord here to
    // produce a armature-up to match the previous armature-down. This extends the message time slightly,
    // but has no other effect. For CW output, a slightly longer silence will be emitted.
    // meInterWord is used rather than meInterElement so that an armature-up sound has enough time to
    // complete playback.
    add_element( meInterWord );
    add_element( meEndOfMessage );
}

// Returns a pointer to the message after preparation for its conversion to Morse elements, and
// following further conversion to a more logging-friendly format.
string
C_text_to_morse::get_message()
{
    text_formatted_ = ( text_[ 0 ] == STX ) ? text_ + 2 : text_;

    for ( unsigned int ii = 0; ii < text_formatted_.length(); ii++ )
    {
        text_formatted_.at( ii ) = tolower( text_formatted_.at( ii ) );
    }

    return ( mnemonic_.length() > 0 ) ? mnemonic_ + " " + text_formatted_ : text_formatted_;
}

eMorseElement
C_text_to_morse::get_element()
{
    if ( morse_element_curr_ < morse_element_count_ )
    {
        return morse_elements_[ morse_element_curr_++ ];
    }

    return meEndOfMessage;
}

void
C_text_to_morse::prepare_message( const string & message, bool add_stx )
{
    text_count_ = 0;

    if ( add_stx )
    {
        text_[ text_count_++ ] = STX;
        text_[ text_count_++ ] = ' ';
    }

    unsigned int message_length = message.length();
    unsigned int message_curr   = 0;
    bool         in_space       = false;

    // If a mnemonic is present, skip over it
    if ( ( message[ 0 ] == '[' ) && ( message[ 4 ] == ']' ) )
    {
        message_curr   = 5;
        mnemonic_      = message.substr( 0, 5 );
    }
    else
    {
        mnemonic_ = "";
    }

    while ( ( text_count_ < ( sizeof( text_ ) - 10 ) ) && ( message_curr < message_length ) )
    {
        if ( ! convert_special_character( message, message_curr, text_, text_count_ ) )
        {
            char ch_curr = toupper( message[ message_curr++ ] );

            if ( isspace( ch_curr ) )
            {
                if ( ! in_space )
                {
                    text_[ text_count_++ ] = ' ';
                    in_space = true;
                }
            }
            else if ( ignore( ch_curr ) )
            {
                continue;
            }
            else if ( ( ch_curr <= MORSE_CHAR_MAX ) && morse_table_[ ( unsigned int ) ch_curr ] )
            {
                 text_[ text_count_++ ] = ch_curr;
                 in_space = false;
            }
        }
    }
    
    text_[ text_count_ ] = '\0';
}

bool
C_text_to_morse::ignore( char ch )
{
    // Ignore non-alphanumeric character if configured
    return ( cfg.c().morse_alphanumeric_only && ( ! isalnum( ch ) ) ) || ( ch > MORSE_CHAR_MAX  );
}

// Check if character (or character sequence) is something that we need to convert to Morse-translatable characters
bool
C_text_to_morse::convert_special_character( const string & message, unsigned int & message_curr,
                                            char *output_buffer,    unsigned int & output_buffer_curr )
{
    // Look up character in special character table    

    special_character_entry *found_entry = NULL;

    for ( special_character_entry *entry = special_character_table; entry->from && ( found_entry == NULL ); entry++ )
    {
        unsigned int msg_idx = message_curr;

        for ( const char *src = entry->from; *src; src++, msg_idx++ )
        {
            if ( *src != message[ msg_idx ] )
            {
                break;
            }

            if ( *( src + 1 ) == '\0' )
            {
                found_entry = entry;        // We have a match
            }
        }
    }

    if ( found_entry )
    {
        // Found a special character or character sequence; convert it to a form we can render in Morse

        for ( const char *dest = found_entry->to; *dest; dest++ )
        {
            output_buffer[ output_buffer_curr++ ] = *dest;
        }

        // Update caller's message offset
        message_curr += strlen( found_entry->from );
    }

    return found_entry != NULL;
}

void
C_text_to_morse::convert_to_morse( char ch )
{
    const char *morse = morse_table_[ ( unsigned int ) ch ];

    for ( unsigned int ii = 0; ii < strlen( morse ); ii++ )
    {
        switch ( morse[ ii ] )
        {
            case '.':
                add_element( meDit );
                break;
            case '-':
                add_element( meDah );
                break;
            default:
                // Should not get here
                break;
        }

        if ( morse[ ii + 1 ] )
        {
            add_element( meInterElement );
        }
    }
}

void
C_text_to_morse::add_element( eMorseElement element )
{
    morse_elements_[ morse_element_count_++ ] = element;
}

// Lookup to Morse code is simplified in that some valid Morse characters are ignored, whilst others are mapped to
// one character e.g. ':' and ';' are both mapped to '-'.
//
// Stx is used internally to denote Start of Transmission, used instead of background noise fade up when the fade up
// is disabled as a notification that a message is about to sent.
//
const char *C_text_to_morse::morse_table_[] =
{
//--------------------------------------------
                    //  ascii   ascii   mapped
                    //  code    char    to
                    //  -----   ----    ------
    NULL,           //  0x00
    NULL,           //  0x01
    "-.-.-",        //  0x02    Stx     SOT
    NULL,           //  0x03
    NULL,           //  0x04
    NULL,           //  0x05
    NULL,           //  0x06
    NULL,           //  0x07
    NULL,           //  0x08
    NULL,           //  0x09
    NULL,           //  0x0A
    NULL,           //  0x0B
    NULL,           //  0x0C
    NULL,           //  0x0D
    NULL,           //  0x0E
    NULL,           //  0x0F
    NULL,           //  0x10
    NULL,           //  0x11
    NULL,           //  0x12
    NULL,           //  0x13
    NULL,           //  0x14
    NULL,           //  0x15
    NULL,           //  0x16
    NULL,           //  0x17
    NULL,           //  0x18
    NULL,           //  0x19
    NULL,           //  0x1A
    NULL,           //  0x1B
    NULL,           //  0x1C
    NULL,           //  0x1D
    NULL,           //  0x1E
    NULL,           //  0x1F
    NULL,           //  0x20    ' '
    NULL,           //  0x21    '!'
    ".-..-.",       //  0x22
    NULL,           //  0x23    '#'
    NULL,           //  0x24    '$'
    NULL,           //  0x25    '%'
    NULL,           //  0x26    '&'
    ".----.",       //  0x27    '''
    "-.--.",        //  0x28    '('
    "-.--.-",       //  0x29    ')'
    "-..-",         //  0x2A    '*'
    ".-.-.",        //  0x2B    '+'
    "--..--",       //  0x2C    ','
    "-....-",       //  0x2D    '-'
    ".-.-.-",       //  0x2E    '.'
    "-..-.",        //  0x2F    '/'
    "-----",        //  0x30    '0'
    ".----",        //  0x31    '1'
    "..---",        //  0x32    '2'
    "...--",        //  0x33    '3'
    "....-",        //  0x34    '4'
    ".....",        //  0x35    '5'
    "-....",        //  0x36    '6'
    "--...",        //  0x37    '7'
    "---..",        //  0x38    '8'
    "----.",        //  0x39    '9'
    "---...",       //  0x3A    ':'
    "---...",       //  0x3B    ';'
    NULL,           //  0x3C    '<'
    "-...-",        //  0x3D    '='
    NULL,           //  0x3E    '>'
    "..--..",       //  0x3F    '?'
    ".--.-.",       //  0x40    '@'
    ".-",           //  0x41    'A'
    "-...",         //  0x42    'B'
    "-.-.",         //  0x43    'C'
    "-..",          //  0x44    'D'
    ".",            //  0x45    'E'
    "..-.",         //  0x46    'F'
    "--.",          //  0x47    'G'
    "....",         //  0x48    'H'
    "..",           //  0x49    'I'
    ".---",         //  0x4A    'J'
    "-.-",          //  0x4B    'K'
    ".-..",         //  0x4C    'L'
    "--",           //  0x4D    'M'
    "-.",           //  0x4E    'N'
    "---",          //  0x4F    'O'
    ".--.",         //  0x50    'P'
    "--.-",         //  0x51    'Q'
    ".-.",          //  0x52    'R'
    "...",          //  0x53    'S'
    "-",            //  0x54    'T'
    "..-",          //  0x55    'U'
    "...-",         //  0x56    'V'
    ".--",          //  0x57    'W'
    "-..-",         //  0x58    'X'
    "-.--",         //  0x59    'Y'
    "--..",         //  0x5A    'Z'
//----------------------------------------------------------
};

const char C_text_to_morse::MORSE_CHAR_MAX = 'Z';

}
