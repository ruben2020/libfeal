//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#ifndef _FEAL_COMMON_H
#define _FEAL_COMMON_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined (_WIN32)
#include "FealCommon_win.h"
#elif defined (__linux__)
#include "FealCommon_posix.h"
#elif defined (__APPLE__)   || defined (__FreeBSD__)   || defined (__NetBSD__) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealCommon_posix.h"
#else
#error "Unsupported operating system"
#endif


class Event;

class EventComm : public Event
{
public:
EventComm() = default;
EventComm( const EventComm & ) = default;
EventComm& operator= ( const EventComm & ) = default;
~EventComm() = default;
EventId_t getId(void);
errenum errnum = FEAL_OK;
handle_t fd = FEAL_INVALID_HANDLE;
int datalen = -1;
};


#endif // _FEAL_COMMON_H
