#ifndef _FEAL_TCP_STREAM_H
#define _FEAL_TCP_STREAM_H

#if defined (_WIN32)
#include "FealTCPStream_winsock.h"
#elif defined (__linux__)
#include "FealTCPStream_posix.h"
#elif defined (__APPLE__)   || defined (__FreeBSD__) || defined (__NetBSD_) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealTCPStream_posix.h"
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include "FealTCPStream_posix.h"
#else
#error "Unsupported operating system"
#endif


#endif // _FEAL_TCP_STREAM_H
