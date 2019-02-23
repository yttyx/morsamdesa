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

// headlines.h

#ifndef headlines_H
#define headlines_H

#include <vector>
#include <string>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "config.h"
#include "curl.h"
#include "datafeed.h"
#include "filter.h"
#include "mutex.h"
#include "thread.h"

using namespace std;

namespace morsamdesa
{

class C_headlines : public C_data_feed, public C_curl_callback, public C_thread
{

public:

    enum eFeedType
    {
        ftRSS,
        ftAtom
    };
    
    C_headlines( const S_url & url, const vector <string> & filters, unsigned int period_min, unsigned int period_max );
    virtual ~C_headlines();

    virtual bool
    start();

    virtual void
    stop();

    virtual bool
    data_ready(){ return false; }

    void
    callback_got_data( const struct curl_data *data );

    bool
    add_filters( const vector< string > & filters );

    void
    get_headlines( vector< C_data_feed_entry > & headlines  );

protected:

    C_headlines() {}
    C_headlines( const C_headlines & ) {}

    virtual void
    process_headlines() = 0;

private:

    bool
    extract_headlines ( const char *xml, size_t xml_len, eFeedType feed_type );
    
    void
    add_headlines( xmlDocPtr doc, xmlNodeSetPtr nodes );

    void
    display_headlines( const char *description, vector< string > & headlines, unsigned int count );

    virtual void
    thread_handler();

protected:

    vector< C_data_feed_entry >   headlines_[ 2 ];
    unsigned int                  headlines_curr_;

    string              url_;                                   // URL of headline feed
    string              mnemonic_;                              // Three character identifier for URL e.g. BBC

    vector < string >   filter_strings_;
    unsigned int        poll_delay_min_;                        // Minimum time between polls (seconds)
    unsigned int        poll_delay_max_;                        // Maximum time between polls (seconds)

    C_mutex             hlock_;

    C_filter            filter_;

private:

    C_curl              *curl_;

};

}

#endif // headlines_H
