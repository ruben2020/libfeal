#include "feal.h"

#if defined (_WIN32)
#define FEAL_EINPROGRESS WSAEWOULDBLOCK
#define MSG_DONTWAIT 0

#else
#define FEAL_EINPROGRESS EINPROGRESS

#endif

feal::EventId_t feal::EvtIncomingConn::getId(void)
{
    return getIdOfType<EvtIncomingConn>();
}

feal::EventId_t feal::EvtDataReadAvail::getId(void)
{
    return getIdOfType<EvtDataReadAvail>();
}

feal::EventId_t feal::EvtDataWriteAvail::getId(void)
{
    return getIdOfType<EvtDataWriteAvail>();
}

feal::EventId_t feal::EvtClientShutdown::getId(void)
{
    return getIdOfType<EvtClientShutdown>();
}

feal::EventId_t feal::EvtServerShutdown::getId(void)
{
    return getIdOfType<EvtServerShutdown>();
}

feal::EventId_t feal::EvtConnectedToServer::getId(void)
{
    return getIdOfType<EvtConnectedToServer>();
}

feal::EventId_t feal::EvtConnectionShutdown::getId(void)
{
    return getIdOfType<EvtConnectionShutdown>();
}



void feal::StreamGeneric::shutdownTool(void)
{
    disconnect_and_reset();
}

feal::errenum feal::StreamGeneric::create_and_bind(feal::ipaddr* fa)
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
    socklen_t length = sizeof(su.in);
    if (fa->family == feal::ipaddr::INET6)
    {
        setipv6only(sockfd);
        length = sizeof(su.in6);
    }
    setnonblocking(sockfd);
    ret = ::bind(sockfd, &(su.sa), length);
    if (ret == FEAL_SOCKET_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    return res;
}

#if defined(unix) || defined(__unix__) || defined(__unix)
feal::errenum feal::StreamGeneric::create_and_bind(struct sockaddr_un* su)
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
    ret = bind(sockfd, (const struct sockaddr*) su, length);
    if (ret == FEAL_SOCKET_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    return res;
}
#endif

feal::errenum feal::StreamGeneric::recv(void *buf,
    uint32_t len, int32_t* bytes, feal::socket_t fd)
{
    errenum res = FEAL_OK;
    if (fd == -1) fd = sockfd;
    ssize_t numbytes = ::recv(fd, buf, (size_t) len, MSG_DONTWAIT);
    if (numbytes == FEAL_SOCKET_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

feal::errenum feal::StreamGeneric::send(void *buf,
    uint32_t len, int32_t* bytes, feal::socket_t fd)
{
    errenum res = FEAL_OK;
    if (fd == -1) fd = sockfd;
    ssize_t numbytes = ::send(fd, buf, (size_t) len, MSG_DONTWAIT);
    if (numbytes == FEAL_SOCKET_ERROR)
    {
        if ((FEAL_GETSOCKETERRNO == EAGAIN)||(FEAL_GETSOCKETERRNO == EWOULDBLOCK))
        {
            do_send_avail_notify(fd);
        }
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

feal::errenum feal::StreamGeneric::disconnect_client(feal::socket_t client_sockfd)
{
    errenum res = FEAL_OK;
    mapReaders.erase(client_sockfd);
    if (do_client_shutdown(client_sockfd) == -1)
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
    return res;
}

feal::errenum feal::StreamGeneric::disconnect_and_reset(void)
{
    for (auto it=mapReaders.begin(); it!=mapReaders.end(); ++it)
        do_client_shutdown(it->first);
    mapReaders.clear();
    errenum res = FEAL_OK;
    if (do_full_shutdown() == -1)
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
    if (serverThread.joinable()) serverThread.join();
    if (connectThread.joinable()) connectThread.join();
    return res;
}

feal::errenum feal::StreamGeneric::getpeername(feal::ipaddr* fa, feal::socket_t fd)
{
    errenum res = FEAL_OK;
    if (fd == -1) fd = sockfd;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    socklen_t length = sizeof(su);
    int ret = ::getpeername(fd, &(su.sa), &length);
    if (ret == FEAL_SOCKET_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    if (fa) ipaddr_posix2feal(&su, fa);
    return res;
}

#if defined(unix) || defined(__unix__) || defined(__unix)
feal::errenum feal::StreamGeneric::getpeername(struct sockaddr_un* su, feal::socket_t fd)
{
    errenum res = FEAL_OK;
    struct sockaddr_un an;
    if (fd == -1) fd = sockfd;
    socklen_t length = sizeof(an);
    int ret = ::getpeername(fd, (struct sockaddr*) su, &length);
    if (ret == FEAL_SOCKET_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    return res;
}

feal::errenum feal::StreamGeneric::getpeereid(uid_t* euid, gid_t* egid)
{
    return getpeereid(sockfd, euid, egid);
}

feal::errenum feal::StreamGeneric::getpeereid(feal::socket_t fd, uid_t* euid, gid_t* egid)
{
    errenum res = FEAL_OK;
    int ret;
#if defined (__linux__)
    socklen_t len;
    struct ucred ucred;
    len = sizeof(struct ucred);
    ret = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len);
    if (ret != FEAL_SOCKET_ERROR)
    {
        *euid = (uid_t) ucred.uid;
        *egid = (gid_t) ucred.gid;
    }
#else
    ret = ::getpeereid(fd, euid, egid);
#endif
    if (ret == FEAL_SOCKET_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETSOCKETERRNO);
        return res;
    }
    return res;
}
#endif

void feal::StreamGeneric::serverLoopLauncher(StreamGeneric *p)
{
    if (p) p->serverLoop();
}

void feal::StreamGeneric::connectLoopLauncher(StreamGeneric *p)
{
    if (p) p->connectLoop();
}

int feal::StreamGeneric::accept_new_conn(void)
{
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    socklen_t socklength = sizeof(su);
    socket_t sock_conn_fd = accept(sockfd, &(su.sa), &socklength);
    std::shared_ptr<EvtIncomingConn> incomingevt = std::make_shared<EvtIncomingConn>();
    incomingevt.get()->client_sockfd = sock_conn_fd;
    if (sock_conn_fd == FEAL_INVALID_SOCKET)
    {
        incomingevt.get()->errnum = static_cast<errenum>(FEAL_GETSOCKETERRNO);
    }
    else
    {
        incomingevt.get()->errnum = FEAL_OK;
    }
    receiveEvent(incomingevt);
    return sock_conn_fd;
}

void feal::StreamGeneric::client_read_avail(feal::socket_t client_sockfd)
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

void feal::StreamGeneric::client_write_avail(feal::socket_t client_sockfd)
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

void feal::StreamGeneric::client_shutdown(feal::socket_t client_sockfd)
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

void feal::StreamGeneric::server_shutdown(void)
{
    std::shared_ptr<EvtServerShutdown> evt = std::make_shared<EvtServerShutdown>();
    receiveEvent(evt);
}

void feal::StreamGeneric::connected_to_server(feal::socket_t fd)
{
    std::shared_ptr<EvtConnectedToServer> evt = std::make_shared<EvtConnectedToServer>();
    evt.get()->server_sockfd = fd;
    receiveEvent(evt);
}

void feal::StreamGeneric::connection_read_avail(void)
{
    std::shared_ptr<EvtDataReadAvail> evt = std::make_shared<EvtDataReadAvail>();
    evt.get()->sockfd = sockfd;
    evt.get()->datalen = datareadavaillen(sockfd);
    receiveEvent(evt);
}

void feal::StreamGeneric::connection_write_avail(void)
{
    std::shared_ptr<EvtDataWriteAvail> evt = std::make_shared<EvtDataWriteAvail>();
    evt.get()->sockfd = sockfd;
    receiveEvent(evt);
}

void feal::StreamGeneric::connection_shutdown(void)
{
    std::shared_ptr<EvtConnectionShutdown> evt = std::make_shared<EvtConnectionShutdown>();
    receiveEvent(evt);
}

void feal::StreamGeneric::receiveEvent(std::shared_ptr<Event> pevt){(void)pevt;}
