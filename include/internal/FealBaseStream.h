#ifndef _FEAL_BASESTREAM_H
#define _FEAL_BASESTREAM_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined (_WIN32)
#include "FealBaseStream_win.h"
#elif defined (__linux__)
#include "FealBaseStream_epoll.h"
#elif defined (__APPLE__)   || defined (__FreeBSD__)   || defined (__NetBSD__) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealBaseStream_kqueue.h"
#else
#error "Unsupported operating system"
#endif


#endif // _FEAL_BASESTREAM_H
