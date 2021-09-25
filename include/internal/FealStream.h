#ifndef _FEAL_STREAM_H
#define _FEAL_STREAM_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif


namespace feal
{

class EvtIncomingConn : public Event
{
public:
EvtIncomingConn() = default;
EvtIncomingConn( const EvtIncomingConn & ) = default;
EvtIncomingConn& operator= ( const EvtIncomingConn & ) = default;
~EvtIncomingConn() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtIncomingConn>();
}
errenum errnum = FEAL_OK;
socket_t client_sockfd = -1;
};

class EvtDataReadAvail : public Event
{
public:
EvtDataReadAvail() = default;
EvtDataReadAvail( const EvtDataReadAvail & ) = default;
EvtDataReadAvail& operator= ( const EvtDataReadAvail & ) = default;
~EvtDataReadAvail() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtDataReadAvail>();
}
socket_t sockfd = -1;
int datalen = -1;
};

class EvtDataWriteAvail : public Event
{
public:
EvtDataWriteAvail() = default;
EvtDataWriteAvail( const EvtDataWriteAvail & ) = default;
EvtDataWriteAvail& operator= ( const EvtDataWriteAvail & ) = default;
~EvtDataWriteAvail() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtDataWriteAvail>();
}
socket_t sockfd = -1;
};


class EvtClientShutdown : public Event
{
public:
EvtClientShutdown() = default;
EvtClientShutdown( const EvtClientShutdown & ) = default;
EvtClientShutdown& operator= ( const EvtClientShutdown & ) = default;
~EvtClientShutdown() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtClientShutdown>();
}
socket_t client_sockfd = -1;
};

class EvtServerShutdown : public Event
{
public:
EvtServerShutdown() = default;
EvtServerShutdown( const EvtServerShutdown & ) = default;
EvtServerShutdown& operator= ( const EvtServerShutdown & ) = default;
~EvtServerShutdown() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtServerShutdown>();
}
};

class EvtConnectedToServer : public Event
{
public:
EvtConnectedToServer() = default;
EvtConnectedToServer( const EvtConnectedToServer & ) = default;
EvtConnectedToServer& operator= ( const EvtConnectedToServer & ) = default;
~EvtConnectedToServer() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtConnectedToServer>();
}
errenum errnum = FEAL_OK;
socket_t server_sockfd = -1;
};

class EvtConnectionShutdown : public Event
{
public:
EvtConnectionShutdown() = default;
EvtConnectionShutdown( const EvtConnectionShutdown & ) = default;
EvtConnectionShutdown& operator= ( const EvtConnectionShutdown & ) = default;
~EvtConnectionShutdown() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtConnectionShutdown>();
}
};

}

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
