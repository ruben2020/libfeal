#ifndef _FEAL_STREAM_POSIX_H
#define _FEAL_STREAM_POSIX_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <cstring>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


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
sockerrenum errnum = S_OK;
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
sockerrenum errnum = S_OK;
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

template<typename Y>
class Stream : public BaseStream<Y>
{
public:
Stream() = default;
Stream( const Stream & ) = default;
Stream& operator= ( const Stream & ) = default;
~Stream() = default;

std::thread serverThread;
std::thread connectThread;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
}

void shutdownTool(void)
{
    disconnect_and_reset();
}

sockerrenum create_and_bind(feal::ipaddr* fa)
{
    sockerrenum res = S_OK;
    int ret;
    if (fa == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(fa, &su);
    if (ret != 1)
    {
        res = static_cast<sockerrenum>(errno);
        return  res;
    }
    BaseStream<Y>::sockfd = socket(fa->family, SOCK_STREAM, 0);
    if (BaseStream<Y>::sockfd == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    socklen_t length = sizeof(su.in);
    if (fa->family == feal::ipaddr::INET6)
    {
        setipv6only(BaseStream<Y>::sockfd);
        length = sizeof(su.in6);
    }
    setnonblocking(BaseStream<Y>::sockfd);
    ret = bind(BaseStream<Y>::sockfd, &(su.sa), length);
    if (ret == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    return res;
}

sockerrenum create_and_bind(struct sockaddr_un* su)
{
    sockerrenum res = S_OK;
    int ret;
    if (su == nullptr) return res;
    BaseStream<Y>::sockfd = socket(su->sun_family, SOCK_STREAM, 0);
    if (BaseStream<Y>::sockfd == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    setnonblocking(BaseStream<Y>::sockfd);
    socklen_t length = sizeof(su->sun_family) + strlen(su->sun_path) + 1;
    ret = bind(BaseStream<Y>::sockfd, (const struct sockaddr*) su, length);
    if (ret == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    return res;
}

sockerrenum create_and_connect(feal::ipaddr* fa)
{
    sockerrenum res = S_OK;
    int ret;
    if (fa == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(fa, &su);
    if (ret != 1)
    {
        res = static_cast<sockerrenum>(errno);
        return  res;
    }
    BaseStream<Y>::sockfd = socket(fa->family, SOCK_STREAM, 0);
    if (BaseStream<Y>::sockfd == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    setnonblocking(BaseStream<Y>::sockfd);
    socklen_t length = sizeof(su.in);
    if (fa->family == feal::ipaddr::INET6)
    {
        length = sizeof(su.in6);
    }
    ret = connect(BaseStream<Y>::sockfd, &(su.sa), length);
    if ((ret == -1) && (errno != EINPROGRESS))
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    else if ((ret == -1) && (errno == EINPROGRESS))
    {
        FEALDEBUGLOG("do_connect_in_progress");
        BaseStream<Y>::do_connect_in_progress();
    }
    else if (ret == 0)
    {
        FEALDEBUGLOG("do_connect_ok");
        BaseStream<Y>::do_connect_ok();
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

sockerrenum create_and_connect(struct sockaddr_un* su)
{
    sockerrenum res = S_OK;
    int ret;
    if (su == nullptr) return res;
    BaseStream<Y>::sockfd = socket(su->sun_family, SOCK_STREAM, 0);
    if (BaseStream<Y>::sockfd == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    setnonblocking(BaseStream<Y>::sockfd);
    socklen_t length = sizeof(su->sun_family) + strlen(su->sun_path) + 1;
    ret = connect(BaseStream<Y>::sockfd, (const struct sockaddr*) su, length);
    if ((ret == -1) && (errno != EINPROGRESS))
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    else if ((ret == -1) && (errno == EINPROGRESS))
    {
        FEALDEBUGLOG("do_connect_in_progress");
        BaseStream<Y>::do_connect_in_progress();
    }
    else if (ret == 0)
    {
        FEALDEBUGLOG("do_connect_ok");
        BaseStream<Y>::do_connect_ok();
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


sockerrenum listen(int backlog = 32)
{
    sockerrenum res = S_OK;
    if (actorptr == nullptr) return res;
    if (::listen(BaseStream<Y>::sockfd, backlog) == -1)
        res = static_cast<sockerrenum>(errno);
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
sockerrenum recv_start(T* p, socket_t client_sockfd)
{
    sockerrenum res = S_OK;
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
        if (BaseStream<Y>::do_client_read_start(client_sockfd) == -1)
            res = static_cast<sockerrenum>(errno);
    }
    return res;
}

sockerrenum recv(void *buf, uint32_t len, int32_t* bytes, socket_t fd = -1)
{
    sockerrenum res = S_OK;
    if (fd == -1) fd = BaseStream<Y>::sockfd;
    ssize_t numbytes = ::recv(fd, buf, (size_t) len, MSG_DONTWAIT);
    if (numbytes == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

sockerrenum send(void *buf, uint32_t len, int32_t* bytes, socket_t fd = -1)
{
    sockerrenum res = S_OK;
    if (fd == -1) fd = BaseStream<Y>::sockfd;
    ssize_t numbytes = ::send(fd, buf, (size_t) len, MSG_DONTWAIT);
    if (numbytes == -1)
    {
        if ((errno == EAGAIN)||(errno == EWOULDBLOCK))
        {
            BaseStream<Y>::do_send_avail_notify(fd);
        }
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

sockerrenum disconnect_client(socket_t client_sockfd)
{
    sockerrenum res = S_OK;
    mapReaders.erase(client_sockfd);
    if (BaseStream<Y>::do_client_shutdown(client_sockfd) == -1)
        res = static_cast<sockerrenum>(errno);
    return res;
}

sockerrenum disconnect_and_reset(void)
{
    for (auto it=mapReaders.begin(); it!=mapReaders.end(); ++it)
        BaseStream<Y>::do_client_shutdown(it->first);
    mapReaders.clear();
    sockerrenum res = S_OK;
    if (BaseStream<Y>::do_full_shutdown() == -1)
        res = static_cast<sockerrenum>(errno);
    if (serverThread.joinable()) serverThread.join();
    if (connectThread.joinable()) connectThread.join();
    return res;
}

sockerrenum getpeername(feal::ipaddr* fa, socket_t fd = -1)
{
    sockerrenum res = S_OK;
    if (fd == -1) fd = BaseStream<Y>::sockfd;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    socklen_t length = sizeof(su);
    int ret = ::getpeername(fd, &(su.sa), &length);
    if (ret == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    if (fa) ipaddr_posix2feal(&su, fa);
    return res;
}

sockerrenum getpeername(struct sockaddr_un* su, socket_t fd = -1)
{
    sockerrenum res = S_OK;
    struct sockaddr_un an;
    if (fd == -1) fd = BaseStream<Y>::sockfd;
    socklen_t length = sizeof(an);
    int ret = ::getpeername(fd, (struct sockaddr*) su, &length);
    if (ret == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    return res;
}

sockerrenum getpeereid(uid_t* euid, gid_t* egid)
{
    return getpeerid(BaseStream<Y>::sockfd, euid, egid);
}

static sockerrenum getpeereid(socket_t fd, uid_t* euid, gid_t* egid)
{
    sockerrenum res = S_OK;
    int ret;
#if defined (__linux__)
    socklen_t len;
    struct ucred ucred;
    len = sizeof(struct ucred);
    ret = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len);
    if (ret != -1)
    {
        *euid = (uid_t) ucred.uid;
        *egid = (gid_t) ucred.gid;
    }
#else
    ret = ::getpeereid(fd, euid, egid);
#endif
    if (ret == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    return res;
}

private:

Y* actorptr = nullptr;
std::map<int, std::weak_ptr<Actor>> mapReaders;


static void serverLoopLauncher(Stream *p)
{
    if (p) p->serverLoop();
}

static void connectLoopLauncher(Stream *p)
{
    if (p) p->connectLoop();
}

int accept_new_conn(void)
{
    if (actorptr == nullptr) return -1;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    socklen_t socklength = sizeof(su);
    socket_t sock_conn_fd = accept(BaseStream<Y>::sockfd, &(su.sa), &socklength);
    std::shared_ptr<EvtIncomingConn> incomingevt = std::make_shared<EvtIncomingConn>();
    incomingevt.get()->client_sockfd = sock_conn_fd;
    if (sock_conn_fd == -1)
    {
        incomingevt.get()->errnum = static_cast<sockerrenum>(errno);
    }
    else
    {
        incomingevt.get()->errnum = S_OK;
    }
    actorptr->receiveEvent(incomingevt);
    return sock_conn_fd;
}

void client_read_avail(socket_t client_sockfd)
{
    std::shared_ptr<EvtDataReadAvail> evt = std::make_shared<EvtDataReadAvail>();
    evt.get()->sockfd = client_sockfd;
    evt.get()->datalen = datareadavaillen(client_sockfd);
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        std::weak_ptr<Actor> wkact = it->second;
        if (wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = wkact.lock();
            act.get()->receiveEvent(evt);
        }
    }
}

void client_write_avail(socket_t client_sockfd)
{
    std::shared_ptr<EvtDataWriteAvail> evt = std::make_shared<EvtDataWriteAvail>();
    evt.get()->sockfd = client_sockfd;
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        std::weak_ptr<Actor> wkact = it->second;
        if (wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = wkact.lock();
            act.get()->receiveEvent(evt);
        }
    }
}

void client_shutdown(socket_t client_sockfd)
{
    std::shared_ptr<EvtClientShutdown> evtclshutdown = std::make_shared<EvtClientShutdown>();
    evtclshutdown.get()->client_sockfd = client_sockfd;
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        std::weak_ptr<Actor> wkact = it->second;
        if (wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = wkact.lock();
            act.get()->receiveEvent(evtclshutdown);
        }
    }
    mapReaders.erase(client_sockfd);
}

void server_shutdown(void)
{
    if (actorptr == nullptr) return;
    std::shared_ptr<EvtServerShutdown> evt = std::make_shared<EvtServerShutdown>();
    actorptr->receiveEvent(evt);
}

void connected_to_server(int fd)
{
    if (actorptr == nullptr) return;
    std::shared_ptr<EvtConnectedToServer> evt = std::make_shared<EvtConnectedToServer>();
    evt.get()->server_sockfd = fd;
    actorptr->receiveEvent(evt);
}

void connection_read_avail(void)
{
    if (actorptr == nullptr) return;
    std::shared_ptr<EvtDataReadAvail> evt = std::make_shared<EvtDataReadAvail>();
    evt.get()->sockfd = BaseStream<Y>::sockfd;
    evt.get()->datalen = datareadavaillen(BaseStream<Y>::sockfd);
    actorptr->receiveEvent(evt);
}

void connection_write_avail(void)
{
    if (actorptr == nullptr) return;
    std::shared_ptr<EvtDataWriteAvail> evt = std::make_shared<EvtDataWriteAvail>();
    evt.get()->sockfd = BaseStream<Y>::sockfd;
    actorptr->receiveEvent(evt);
}

void connection_shutdown(void)
{
    if (actorptr == nullptr) return;
    std::shared_ptr<EvtConnectionShutdown> evt = std::make_shared<EvtConnectionShutdown>();
    actorptr->receiveEvent(evt);
}

};


} // namespace feal


#endif // _FEAL_STREAM_POSIX_H
