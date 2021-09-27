
#if defined (_WIN32)
#include "FealBaseStream_win.cpp"
#elif defined (__linux__)
#include "FealBaseStream_epoll.cpp"
#elif defined (__APPLE__)   || defined (__FreeBSD__) || defined (__NetBSD_) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealBaseStream_kqueue.cpp"
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include "FealBaseStream_kqueue.cpp"
#else
#error "Unsupported operating system"
#endif
