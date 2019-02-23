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

// curl.h
#ifndef CURL_H
#define CURL_H

#include <string>

#include <curl/curl.h>

using namespace std;


namespace morsamdesa
{

struct curl_data
{
    char   *memory;
    size_t size;
};

class C_curl_callback
{
public:
    virtual void callback_got_data( const struct curl_data *data ) = 0;
};


class C_curl : public C_curl_callback
{
public:
    C_curl( const string & url );
    virtual ~C_curl();

    void
    set_callback( C_curl_callback *cb );

    void
    get_data();
    
    const char *data();

protected:
    C_curl() {}
    C_curl( const C_curl & ) {}

    static size_t
    write_memory_callback( void *contents, size_t size, size_t nmemb, void *userp );

    void
    free_chunk();

    virtual void
    callback_got_data( const struct curl_data *data ){}

private:
    string     url_;

    CURL            *curl_handle_;

    struct curl_data chunk_;

    static int instance_count_;

private:
    C_curl_callback *cb_;

};

}

#endif // CURL_H
