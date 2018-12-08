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

// curl.cpp
//
// reference:  http://curl.haxx.se/libcurl/c/getinmemory.html
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curl.h"
#include "log.h"
#include "misc.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;


C_curl::C_curl( const string & url )
{
    assert( url.length() > 0  );

    url_ = url;

    chunk_.memory = NULL;
    chunk_.size   = 0;

    if ( instance_count_ == 0 )
    {
        curl_global_init( CURL_GLOBAL_ALL );
    }

    instance_count_++;
}

C_curl::~C_curl()
{
    free_chunk();

    instance_count_--;

    if ( instance_count_ == 0 )
    {
        curl_global_cleanup();
    }
}

void
C_curl::set_callback(C_curl_callback *cb)
{
    assert( cb );

    cb_ = cb;
}

void
C_curl::get_data()
{
    assert( url_.length() > 0 );

    // Initialise the curl session
    curl_handle_ = curl_easy_init();

    CURLcode curl_result;

    curl_result = curl_easy_setopt( curl_handle_, CURLOPT_URL, url_.c_str() );

    // Send all data to the function write_memory_callback
    curl_result = curl_easy_setopt( curl_handle_, CURLOPT_WRITEFUNCTION, C_curl::write_memory_callback );

    // We pass our 'chunk' struct to the callback function
    curl_result = curl_easy_setopt( curl_handle_, CURLOPT_WRITEDATA, ( void * ) &chunk_ );

    // Some servers don't like requests that are made without a user-agent field, so we provide one
    curl_result = curl_easy_setopt( curl_handle_, CURLOPT_USERAGENT, "libcurl-agent/1.0" );

    // Fetch the data
    curl_result = curl_easy_perform( curl_handle_ );

    // Check for errors
    if ( curl_result == CURLE_OK )
    {
        if ( cb_ )
        {
            cb_->callback_got_data( &chunk_ );
        }
    }
    else
    {
        log_writeln_fmt( C_log::LL_WARNING, "curl_easy_perform() failed: %s",  curl_easy_strerror( curl_result ) );
    }
    
    curl_easy_cleanup( curl_handle_ );

    free_chunk();
}

const char *
C_curl::data()
{
    return chunk_.memory;
}

void
C_curl::free_chunk()
{
    if ( chunk_.memory )
    {
        free( chunk_.memory );
        chunk_.memory = NULL;
        chunk_.size   = 0;
    }
}

size_t
C_curl::write_memory_callback( void *contents, size_t size, size_t nmemb, void *userp )
{
    size_t realsize = size * nmemb;
    struct curl_data *mem = ( struct curl_data * ) userp;

    mem->memory = ( char * ) realloc( mem->memory, mem->size + realsize + 1 );
    if ( mem->memory == NULL )
    {
        log_writeln( C_log::LL_ERROR, "not enough memory (realloc returned NULL)" );
        return 0;
    }

    memcpy( &( mem->memory[ mem->size ] ), contents, realsize );
    mem->size += realsize;
    mem->memory[ mem->size ] = 0;

    return realsize;
}

int C_curl::instance_count_ = 0;

}
