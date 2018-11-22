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

// log.h

#ifndef log_H
#define log_H

#include "stdarg.h"
#include <string>

#include "mutex.h"

using namespace std;


namespace morsamdesa
{
    // No newline at end of line
    #define log_write( level, line )                log.write_line( level, __FILE__, __LINE__, false, line )
    #define log_write_fmt( level, format, ...)      log.write_line( level, __FILE__, __LINE__, false, format, __VA_ARGS__ )

    // Newline at end of line
    #define log_writeln( level, line )              log.write_line( level, __FILE__, __LINE__, true, line )
    #define log_writeln_fmt( level, format, ...)    log.write_line( level, __FILE__, __LINE__, true, format, __VA_ARGS__ )

class C_log
{
public:

    enum eLogLevel
    {
        LL_NONE         = 0,
        LL_ERROR        = 1,
        LL_WARNING      = 2,
        LL_INFO         = 3,
        LL_VERBOSE_1    = 4,
        LL_VERBOSE_2    = 5,
        LL_VERBOSE_3    = 6
    };

    C_log();
    ~C_log();

    void
    initialise( int level, bool datetime );

    void
    write_line( eLogLevel level, const char *file, int line, bool newline, const char *format, ... );

    eLogLevel
    log_level() { return level_; }

    static string
    format_string( const char *fmt, ... );

private:

    string
    get_datetime();

    static string
    format_string_internal( bool fileline, const char *file, int line, const char *fmt, va_list arg_ptr );

private:

    bool        datetime_;
    bool        fileline_;
    eLogLevel   level_;

    C_mutex     log_lock_;
};

}

#endif // log_H
