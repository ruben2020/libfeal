#include "feal.h"

#if defined (_WIN32)
#include "FealStream_iocp.cpp"
#elif defined (__linux__)
#include "FealStream_epoll.cpp"
#elif defined (__APPLE__)   || defined (__FreeBSD__)   || defined (__NetBSD__) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealStream_kqueue.cpp"
#else
#error "Unsupported operating system"
#endif
