#ifndef _FEAL_STREAM_H
#define _FEAL_STREAM_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined (_WIN32)
#include "FealStream_winsock.h"
#elif defined (__linux__)
#include "FealStream_posix.h"
#elif defined (__APPLE__)   || defined (__FreeBSD__) || defined (__NetBSD_) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealStream_posix.h"
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include "FealStream_posix.h"
#else
#error "Unsupported operating system"
#endif


#endif // _FEAL_STREAM_H
