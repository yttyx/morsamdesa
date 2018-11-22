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

// headlines.cpp
//

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "misc.h"
#include "headlines.h"

using namespace  morsamdesa;

namespace morsamdesa
{

extern C_log log;


C_headlines::C_headlines( const string & url, const vector <string> & filters, unsigned int period_min, unsigned int period_max )
    : url_( url ),
      filter_strings_( filters ),
      poll_delay_min_( period_min ),
      poll_delay_max_( period_max )
{
    assert( url.length() > 0 );
    assert( poll_delay_max_ > poll_delay_min_ );

    headlines_curr_ = 0;
    curl_           = new C_curl( url );
    
    curl_->set_callback( this );
}

C_headlines::~C_headlines()
{
    if ( curl_ )
    {
        delete curl_;
    }
}

// -----------------------------------------------------------------------------------
// Main thread code
// -----------------------------------------------------------------------------------

bool
C_headlines::start()
{
    // Start up background thread
    return thread_start();
}

void
C_headlines::stop()
{
    C_data_feed::stop();

    thread_await_exit();
}

bool
C_headlines::add_filters( const vector< string > & filters )
{
    bool worked = filter_.add_filters( filters );

    return worked;
}

void
C_headlines::get_headlines( vector< string > & headlines )
{
    hlock_.lock();

    headlines.clear();

    for ( unsigned int ii = 0; ii < headlines_[ headlines_curr_].size(); ii++ )
    {
        headlines.push_back( headlines_[ headlines_curr_ ][ ii ] );
    }

    hlock_.unlock();
}

// -----------------------------------------------------------------------------------
// Background thread code
// -----------------------------------------------------------------------------------

void
C_headlines::thread_handler()
{
    // When this method ends, the thread will terminate
    while ( ! abort_ )
    {
        curl_->get_data();

        // Wait for some random time between poll_delay_min_ and poll_delay_max_ seconds
        unsigned int diff = poll_delay_max_ - poll_delay_min_;

        // Random value between 0 and ( diff - 1 )
        unsigned int delay_offset = ( unsigned int ) ( rand() % diff );

        unsigned int this_delay       = ( poll_delay_min_ + delay_offset ) * 1000;    // mS
        unsigned int fifty_ms_periods = this_delay / 20;

        for ( unsigned int ii = 0; ( ii < fifty_ms_periods ) && ( ! abort_ ); ii++ )
        {
            delay( 50 );
        }
    }
}

void
C_headlines::callback_got_data( const struct curl_data *data )
{
    assert( data );

    // Try interpreting the data first as an RSS feed, then as an Atom feed
    // if the RSS parsing does not work.
    if ( ! extract_headlines( data->memory, data->size, ftRSS ) )
    {
        extract_headlines( data->memory, data->size, ftAtom );
    }

    process_headlines();
}

bool
C_headlines::extract_headlines( const char *xml, size_t xml_len, eFeedType feed_type )
{
    assert( xml );

    xmlDocPtr          doc             = NULL;
    xmlXPathContextPtr xpathContextPtr = NULL;
    xmlXPathObjectPtr  xpathObjectPtr  = NULL;

    bool worked = false;

    try
    {
        xmlInitParser();

        const xmlChar      *xpathExpr = ( feed_type == ftRSS ) ? BAD_CAST "//rss/channel/item/title"
                                                               : BAD_CAST "//x:feed/x:entry/x:title";

        // Parse XML. Because the document is in memory, it has no base per RFC 2396,
        // so the "noname.xml" argument will serve as its base.
        doc = xmlReadMemory( xml, xml_len, "noname.xml", NULL, 0 );

        if ( doc )
        {
            xpathContextPtr = xmlXPathNewContext( doc );

            if ( xpathContextPtr )
            {
                xmlChar *prefix = BAD_CAST "x";
                xmlChar *href   = BAD_CAST "http://www.w3.org/2005/Atom";

                bool ok = ( feed_type == ftAtom ) ? xmlXPathRegisterNs( xpathContextPtr, prefix, href ) == 0 : true;

                if ( ok )
                {
                    xpathObjectPtr = xmlXPathEvalExpression( xpathExpr, xpathContextPtr );

                    if ( xpathObjectPtr && xpathObjectPtr->nodesetval )
                    {
                        if ( xpathObjectPtr->nodesetval->nodeNr > 0 )
                        {
                            add_headlines( doc, xpathObjectPtr->nodesetval );
                            worked = true;
                        }
                    }
                }
            }
        }
    }
    catch ( exception & ex )
    {
        log_writeln_fmt( C_log::LL_ERROR, "Exception parsing headlines XML: %s", ex.what() );
    }
    catch ( ... )
    {
        log_writeln( C_log::LL_ERROR, "Exception parsing headlines XML" );
    }

    if ( xpathObjectPtr )
    {
        xmlXPathFreeObject( xpathObjectPtr );
    }
    else
    {
        log_writeln( C_log::LL_ERROR, "C_headlines::extract_headlines(): xpath request failed." );
    }
    if ( xpathContextPtr )
    {
        xmlXPathFreeContext( xpathContextPtr );
    }
    else
    {
        log_writeln( C_log::LL_ERROR, "C_headlines::extract_headlines(): Failed to create xpathContextPtr." );
    }
    if ( doc )
    {
        xmlFreeDoc(doc);
    }
    else
    {
        log_writeln( C_log::LL_ERROR, "C_headlines::extract_headlines(): XML parsing failed." );
    }

    xmlCleanupParser();

    return worked;
}

void
C_headlines::add_headlines( xmlDocPtr doc, xmlNodeSetPtr nodes )
{
    assert ( nodes );

    hlock_.lock();

    headlines_curr_ = ( headlines_curr_ == 1 ) ? 0 : 1;
    headlines_[ headlines_curr_ ].clear();

    xmlNodePtr node;

    for ( int ii = 0; ii < nodes->nodeNr; ii++ )
    {
        assert( nodes->nodeTab[ ii ] );
        
        if ( nodes->nodeTab[ ii ]->type == XML_ELEMENT_NODE )
        {
            node = nodes->nodeTab[ ii ];
            
            xmlChar *key = xmlNodeListGetString( doc, node->xmlChildrenNode, 1 );

            string key_str( ( const char * ) key );

            if ( filter_.is_acceptable( key_str.c_str() ) )
            {
                headlines_[ headlines_curr_ ].push_back( key_str );
            }

            xmlFree( key );
        }
    }

    hlock_.unlock();
}

/** \brief Show headlines

    @param[in]      headlines: list of headlines
    @param[in]      count    : maximum number of headlines to log
*/
void
C_headlines::display_headlines( const char *description, vector< string> & headlines, unsigned int count )
{
    unsigned int ii_max = min( count, headlines.size() );

    log_writeln( C_log::LL_INFO, "------------------------------------------------" );
    log_writeln_fmt( C_log::LL_INFO, "%s", description );

    for ( unsigned int ii = 0; ii < ii_max; ii++ )
    {
        log_writeln_fmt( C_log::LL_INFO, "%s", headlines[ ii ].c_str() );
    }

    log_writeln( C_log::LL_INFO, "------------------------------------------------" );
}

}
