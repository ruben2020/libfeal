#include "feal.h"

#if defined (_WIN32)
#include "FealTCPStream_winsock.cpp"
#elif defined (__linux__)
#include "FealTCPStream_posix.cpp"
#elif defined (__APPLE__)   || defined (__FreeBSD__) || defined (__NetBSD_) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealTCPStream_posix.cpp"
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include "FealTCPStream_posix.cpp"
#else
#error "Unsupported operating system"
#endif
