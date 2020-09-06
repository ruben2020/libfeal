#ifndef _FEAL_STREAM_H
#define _FEAL_STREAM_H


#if defined (_WIN32)
#include "FealStream_iocp.h"
#elif defined (__linux__)
#include "FealStream_epoll.h"
#elif defined (__APPLE__)   || defined (__FreeBSD__)   || defined (__NetBSD__) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealStream_kqueue.h"
#else
#error "Unsupported operating system"
#endif


#endif // _FEAL_STREAM_H
