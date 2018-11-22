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

// feed_wordlist.cpp
//

#include <algorithm>
#include <assert.h>

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "feed_wordlist.h"
#include "log.h"
#include "misc.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;

C_feed_wordlist::C_feed_wordlist( const string & wordlist_file, bool repeat, bool random_order )
    : wordlist_file_( wordlist_file ),
      repeat_ ( repeat ),
      random_order_ ( random_order )
{
    word_curr_    = 0;
    word_max_     = 0;
    word_count_   = 0;

    no_more_data_ = false;
}

C_feed_wordlist::~C_feed_wordlist()
{
    log_writeln( C_log::LL_VERBOSE_3, "C_feed_wordlist destructor" );
}

/** \brief Make available Send a series of words from a file contains a list of them

    Don't re-use a word until all other words in a list have been consumed.
*/
bool
C_feed_wordlist::start()
{
    if ( wordlist_file_.length() == 0 )
    {
        log_writeln( C_log::LL_ERROR, "No wordlist file supplied" );
        return false;
    }

    // Just use the first file in the vector for the time being
    if ( ! populate_wordlist( wordlist_file_ ) )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Failed to read wordlist file '%s'", wordlist_file_.c_str() );
        return false;
    }

    word_max_ =  wordlist_.size();

    if ( word_max_ == 0 )
    {
        log_writeln_fmt( C_log::LL_ERROR, "No words in wordlist file '%s'", wordlist_file_.c_str() );
        return false;
    }

    log_writeln_fmt( C_log::LL_INFO, "File: %s, %u words available", wordlist_file_.c_str(), word_max_ );

    random_number_ = new C_random( word_max_ );

    return true;
}

bool
C_feed_wordlist::data_ready()
{
    return true;
}

bool
C_feed_wordlist::read( string & str )
{
    unsigned int new_word_index = random_order_ ? random_number_->not_in_last_n() : word_curr_++;

    if ( word_curr_ >= ( word_max_ ) )
    {
        word_curr_ = 0;
    }

    str = wordlist_[ new_word_index ];

    if ( ( ++word_count_ % word_max_ ) == 0 )
    {
        if ( ! repeat_ )
        {
            log_writeln( C_log::LL_INFO, "Wordlist: All words sent" );
            no_more_data_ = true;
        }
    }

    return true;
}

bool
C_feed_wordlist::populate_wordlist( const string & filename )
{
    ifstream wordlist_stream( ( const char * ) filename.c_str() );

    //log_writeln_fmt( C_log::LL_INFO, "Reading from wordlist file %s", filename.c_str() );
    
    if ( wordlist_stream.is_open() )
    {
        string line;
        
        while ( getline( wordlist_stream, line ) )
        {
            rtrim_string( line );

            wordlist_.push_back( line );
        }

        wordlist_stream.close();

        return true;
    }

    return false;
}

}
