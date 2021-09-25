#ifndef _FEAL_DATAGRAM_H
#define _FEAL_DATAGRAM_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{

class EvtDgramReadAvail : public Event
{
public:
EvtDgramReadAvail() = default;
EvtDgramReadAvail( const EvtDgramReadAvail & ) = default;
EvtDgramReadAvail& operator= ( const EvtDgramReadAvail & ) = default;
~EvtDgramReadAvail() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtDgramReadAvail>();
}
socket_t sockfd = -1;
int datalen = -1;
};

class EvtDgramWriteAvail : public Event
{
public:
EvtDgramWriteAvail() = default;
EvtDgramWriteAvail( const EvtDgramWriteAvail & ) = default;
EvtDgramWriteAvail& operator= ( const EvtDgramWriteAvail & ) = default;
~EvtDgramWriteAvail() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtDgramWriteAvail>();
}
socket_t sockfd = -1;
};


class EvtSockErr : public Event
{
public:
EvtSockErr() = default;
EvtSockErr( const EvtSockErr & ) = default;
EvtSockErr& operator= ( const EvtSockErr & ) = default;
~EvtSockErr() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtSockErr>();
}

};

}


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
