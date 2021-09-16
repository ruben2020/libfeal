#ifndef _FEAL_DATAGRAM_H
#define _FEAL_DATAGRAM_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined (_WIN32)
#include "FealDatagram_winsock.h"
#elif defined (__linux__)
#include "FealDatagram_posix.h"
#elif defined (__APPLE__)   || defined (__FreeBSD__) || defined (__NetBSD_) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealDatagram_posix.h"
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include "FealDatagram_posix.h"
#else
#error "Unsupported operating system"
#endif


#endif // _FEAL_DATAGRAM_H
