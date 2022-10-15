//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#ifndef _FEAL_STREAM_H
#define _FEAL_STREAM_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined (_WIN32)
#define FEAL_STREAM_EINPROGRESS WSAEWOULDBLOCK
#else
#define FEAL_STREAM_EINPROGRESS EINPROGRESS
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
EventId_t getId(void);
errenum errnum = FEAL_OK;
socket_t client_sockfd = FEAL_INVALID_SOCKET;
};

class EvtDataReadAvail : public Event
{
public:
EvtDataReadAvail() = default;
EvtDataReadAvail( const EvtDataReadAvail & ) = default;
EvtDataReadAvail& operator= ( const EvtDataReadAvail & ) = default;
~EvtDataReadAvail() = default;
EventId_t getId(void);
socket_t sockfd = FEAL_INVALID_SOCKET;
int datalen = -1;
};

class EvtDataWriteAvail : public Event
{
public:
EvtDataWriteAvail() = default;
EvtDataWriteAvail( const EvtDataWriteAvail & ) = default;
EvtDataWriteAvail& operator= ( const EvtDataWriteAvail & ) = default;
~EvtDataWriteAvail() = default;
EventId_t getId(void);
socket_t sockfd = FEAL_INVALID_SOCKET;
};


class EvtClientShutdown : public Event
{
public:
EvtClientShutdown() = default;
EvtClientShutdown( const EvtClientShutdown & ) = default;
EvtClientShutdown& operator= ( const EvtClientShutdown & ) = default;
~EvtClientShutdown() = default;
EventId_t getId(void);
socket_t client_sockfd = FEAL_INVALID_SOCKET;
};

class EvtServerShutdown : public Event
{
public:
EvtServerShutdown() = default;
EvtServerShutdown( const EvtServerShutdown & ) = default;
EvtServerShutdown& operator= ( const EvtServerShutdown & ) = default;
~EvtServerShutdown() = default;
EventId_t getId(void);
};

class EvtConnectedToServer : public Event
{
public:
EvtConnectedToServer() = default;
EvtConnectedToServer( const EvtConnectedToServer & ) = default;
EvtConnectedToServer& operator= ( const EvtConnectedToServer & ) = default;
~EvtConnectedToServer() = default;
EventId_t getId(void);
errenum errnum = FEAL_OK;
socket_t server_sockfd = FEAL_INVALID_SOCKET;
};

class EvtConnectionShutdown : public Event
{
public:
EvtConnectionShutdown() = default;
EvtConnectionShutdown( const EvtConnectionShutdown & ) = default;
EvtConnectionShutdown& operator= ( const EvtConnectionShutdown & ) = default;
~EvtConnectionShutdown() = default;
EventId_t getId(void);
};


class StreamGeneric : public BaseStream
{
public:
StreamGeneric() = default;
StreamGeneric( const StreamGeneric & ) = default;
StreamGeneric& operator= ( const StreamGeneric & ) = default;
~StreamGeneric() = default;

void shutdownTool(void);

#if defined(unix) || defined(__unix__) || defined(__unix)
errenum create_and_bind(struct sockaddr_un* su);
errenum getpeername(struct sockaddr_un* su, socket_t fd = -1);
errenum getpeereid(uid_t* euid, gid_t* egid);
static errenum getpeereid(socket_t fd, uid_t* euid, gid_t* egid);
#endif

errenum create_and_bind(feal::ipaddr* fa);
errenum recv(void *buf, uint32_t len, int32_t* bytes, socket_t fd = FEAL_INVALID_SOCKET);
errenum send(void *buf, uint32_t len, int32_t* bytes, socket_t fd = FEAL_INVALID_SOCKET);
errenum disconnect_client(socket_t client_sockfd);
errenum disconnect_and_reset(void);
errenum getpeername(feal::ipaddr* fa, socket_t fd = FEAL_INVALID_SOCKET);

protected:

std::thread serverThread;
std::thread connectThread;
std::map<int, std::weak_ptr<Actor>> mapReaders;

static void serverLoopLauncher(StreamGeneric *p);
static void connectLoopLauncher(StreamGeneric *p);
virtual void receiveEvent(std::shared_ptr<Event> pevt);

int  accept_new_conn(void);
void client_read_avail(socket_t client_sockfd);
void client_write_avail(socket_t client_sockfd);
void client_shutdown(socket_t client_sockfd);
void server_shutdown(void);
void connected_to_server(socket_t fd);
void connection_read_avail(void);
void connection_write_avail(void);
void connection_shutdown(void);

};



template<typename Y>
class Stream : public StreamGeneric
{
public:
Stream() = default;
Stream( const Stream & ) = default;
Stream& operator= ( const Stream & ) = default;
~Stream() = default;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
}

errenum create_and_connect(feal::ipaddr* fa)
{
    errenum res = FEAL_OK;
    int ret;
    if (fa == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(fa, &su);
    if (ret == FEAL_SOCKET_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return  res;
    }
    sockfd = socket(fa->family, SOCK_STREAM, 0);
    if (sockfd == FEAL_INVALID_SOCKET)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    setnonblocking(sockfd);
    socklen_t length = sizeof(su.in);
    if (fa->family == feal::ipaddr::INET6)
    {
        length = sizeof(su.in6);
    }
    ret = connect(sockfd, &(su.sa), length);
    if ((ret == FEAL_SOCKET_ERROR) &&
        (FEAL_GETSOCKETERRNO != FEAL_STREAM_EINPROGRESS))
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    else if ((ret == FEAL_SOCKET_ERROR) &&
        (FEAL_GETSOCKETERRNO == FEAL_STREAM_EINPROGRESS))
    {
        FEALDEBUGLOG("do_connect_in_progress");
        do_connect_in_progress();
    }
    else if (ret == 0)
    {
        FEALDEBUGLOG("do_connect_ok");
        do_connect_ok();
    }
    EvtConnectedToServer ects;
    EvtDataReadAvail edra;
    EvtDataWriteAvail edwa;
    EvtConnectionShutdown ecs;
    actorptr->addEvent(actorptr, ects);
    actorptr->addEvent(actorptr, edra);
    actorptr->addEvent(actorptr, edwa);
    actorptr->addEvent(actorptr, ecs);
    connectThread = std::thread(&connectLoopLauncher, this);
    return res;
}

#if defined(unix) || defined(__unix__) || defined(__unix)
errenum create_and_connect(struct sockaddr_un* su)
{
    errenum res = FEAL_OK;
    int ret;
    if (su == nullptr) return res;
    sockfd = socket(su->sun_family, SOCK_STREAM, 0);
    if (sockfd == FEAL_INVALID_SOCKET)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    setnonblocking(sockfd);
    socklen_t length = sizeof(su->sun_family) + strlen(su->sun_path) + 1;
    ret = connect(sockfd, (const struct sockaddr*) su, length);
    if ((ret == FEAL_SOCKET_ERROR) &&
        (FEAL_GETSOCKETERRNO != FEAL_STREAM_EINPROGRESS))
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    else if ((ret == FEAL_SOCKET_ERROR) &&
        (FEAL_GETSOCKETERRNO == FEAL_STREAM_EINPROGRESS))
    {
        FEALDEBUGLOG("do_connect_in_progress");
        do_connect_in_progress();
    }
    else if (ret == 0)
    {
        FEALDEBUGLOG("do_connect_ok");
        do_connect_ok();
    }
    EvtConnectedToServer ects;
    EvtDataReadAvail edra;
    EvtDataWriteAvail edwa;
    EvtConnectionShutdown ecs;
    actorptr->addEvent(actorptr, ects);
    actorptr->addEvent(actorptr, edra);
    actorptr->addEvent(actorptr, edwa);
    actorptr->addEvent(actorptr, ecs);
    connectThread = std::thread(&connectLoopLauncher, this);
    return res;
}
#endif

errenum listen(int backlog = 32)
{
    errenum res = FEAL_OK;
    if (::listen(sockfd, backlog) == FEAL_SOCKET_ERROR)
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
    else
    {
        EvtIncomingConn eic;
        EvtServerShutdown ess;
        actorptr->addEvent(actorptr, eic);
        actorptr->addEvent(actorptr, ess);
        serverThread = std::thread(&serverLoopLauncher, this);
    }
    return res;
}

template<class T>
errenum recv_start(T* p, socket_t client_sockfd)
{
    errenum res = FEAL_OK;
    std::weak_ptr<Actor> wkact;
    if (p)
    {
        wkact = p->shared_from_this();
        EvtDataReadAvail edra;
        EvtDataWriteAvail edwa;
        EvtClientShutdown ecsd;
        p->addEvent(p, edra);
        p->addEvent(p, edwa);
        p->addEvent(p, ecsd);
    }
    auto it = mapReaders.find(client_sockfd);
    if (it == mapReaders.end())
    {
        mapReaders[client_sockfd] = wkact;
        setnonblocking(client_sockfd);
        if (do_client_read_start(client_sockfd) == -1)
            res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
    }
    return res;
}

protected:

void receiveEvent(std::shared_ptr<Event> pevt)
{
    if (actorptr) actorptr->receiveEvent(pevt);
}

private:
Y* actorptr = nullptr;


};

} // namespace feal


#endif // _FEAL_STREAM_H
