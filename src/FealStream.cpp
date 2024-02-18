//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include "feal.h"

#if defined (_WIN32)
#define BUFCAST(x) (char *) x
#define CLOSESOCKET(x) closesocket(x)

#else
#define BUFCAST(x) (void *) x
#define CLOSESOCKET(x) close(x)

#endif


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
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return  res;
    }
    sockfd = socket(fa->family, SOCK_STREAM, 0);
    if (sockfd == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
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
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
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
    if (sockfd == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    setnonblocking(sockfd);
    socklen_t length = sizeof(su->sun_family) + strlen(su->sun_path) + 1;
    ret = bind(sockfd, (const struct sockaddr*) su, length);
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    return res;
}
#endif

feal::errenum feal::StreamGeneric::recv(void *buf,
    uint32_t len, int32_t* bytes, feal::handle_t fd)
{
    errenum res = FEAL_OK;
    if (fd == FEAL_INVALID_HANDLE) fd = sockfd;
    ssize_t numbytes = ::recv(fd, BUFCAST(buf), (size_t) len, MSG_DONTWAIT);
#if defined (_WIN32)
    do_client_read_start(fd);
#endif
    if (numbytes == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

feal::errenum feal::StreamGeneric::send(void *buf,
    uint32_t len, int32_t* bytes, feal::handle_t fd)
{
    errenum res = FEAL_OK;
    if (fd == FEAL_INVALID_HANDLE) fd = sockfd;
    ssize_t numbytes = ::send(fd, BUFCAST(buf), (size_t) len, MSG_DONTWAIT);
    if (numbytes == FEAL_HANDLE_ERROR)
    {
        if ((FEAL_GETHANDLEERRNO == EAGAIN)||(FEAL_GETHANDLEERRNO == EWOULDBLOCK))
        {
            do_send_avail_notify(fd);
        }
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

feal::errenum feal::StreamGeneric::disconnect_client(feal::handle_t client_sockfd)
{
    errenum res = FEAL_OK;
    mapReaders.erase(client_sockfd);
    if (do_client_shutdown(client_sockfd) == -1)
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
    return res;
}

feal::errenum feal::StreamGeneric::disconnect_and_reset(void)
{
    for (auto it=mapReaders.begin(); it!=mapReaders.end(); ++it)
        do_client_shutdown(it->first);
    mapReaders.clear();
    errenum res = FEAL_OK;
    if (do_full_shutdown() == -1)
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
    if (serverThread.joinable()) serverThread.join();
    if (connectThread.joinable()) connectThread.join();
    return res;
}

feal::errenum feal::StreamGeneric::getpeername(feal::ipaddr* fa, feal::handle_t fd)
{
    errenum res = FEAL_OK;
    if (fd == FEAL_INVALID_HANDLE) fd = sockfd;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    socklen_t length = sizeof(su);
    int ret = ::getpeername(fd, &(su.sa), &length);
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (fa) ipaddr_posix2feal(&su, fa);
    return res;
}

#if defined(unix) || defined(__unix__) || defined(__unix)
feal::errenum feal::StreamGeneric::getpeername(struct sockaddr_un* su, feal::handle_t fd)
{
    errenum res = FEAL_OK;
    struct sockaddr_un an;
    if (fd == -1) fd = sockfd;
    socklen_t length = sizeof(an);
    int ret = ::getpeername(fd, (struct sockaddr*) su, &length);
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    return res;
}

feal::errenum feal::StreamGeneric::getpeereid(uid_t* euid, gid_t* egid)
{
    return getpeereid(sockfd, euid, egid);
}

feal::errenum feal::StreamGeneric::getpeereid(feal::handle_t fd, uid_t* euid, gid_t* egid)
{
    errenum res = FEAL_OK;
    int ret;
#if defined (__linux__)
    socklen_t len;
    struct ucred ucred;
    len = sizeof(struct ucred);
    ret = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len);
    if (ret != FEAL_HANDLE_ERROR)
    {
        *euid = (uid_t) ucred.uid;
        *egid = (gid_t) ucred.gid;
    }
#else
    ret = ::getpeereid(fd, euid, egid);
#endif
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
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
    handle_t sock_conn_fd = accept(sockfd, &(su.sa), &socklength);
    errenum errnum = FEAL_OK;
    if (sock_conn_fd == FEAL_INVALID_HANDLE)
    {
        errnum = static_cast<errenum>(FEAL_GETHANDLEERRNO);
    }
    receiveEventIncomingConn(errnum, sock_conn_fd, -1);
    return sock_conn_fd;
}

void feal::StreamGeneric::client_read_avail(feal::handle_t client_sockfd)
{
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        clientst cst = it->second;
        if (cst.wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = cst.wkact.lock();
            auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(cst.evtclientreadavail));
            itw.get()->errnum = FEAL_OK;
            itw.get()->fd = client_sockfd;
            itw.get()->datalen = datareadavaillen(client_sockfd);
            act.get()->receiveEvent(itw);
        }
    }
}

void feal::StreamGeneric::client_write_avail(feal::handle_t client_sockfd)
{
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        clientst cst = it->second;
        if (cst.wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = cst.wkact.lock();
            auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(cst.evtclientwriteavail));
            itw.get()->errnum = FEAL_OK;
            itw.get()->fd = client_sockfd;
            itw.get()->datalen = -1;
            act.get()->receiveEvent(itw);
        }
    }
}

void feal::StreamGeneric::client_shutdown(feal::handle_t client_sockfd)
{
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        clientst cst = it->second;
        if (cst.wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = cst.wkact.lock();
            auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(cst.evtclientshutdown));
            itw.get()->errnum = FEAL_OK;
            itw.get()->fd = client_sockfd;
            itw.get()->datalen = -1;
            act.get()->receiveEvent(itw);
        }
    }
    mapReaders.erase(client_sockfd);
}

void feal::StreamGeneric::server_shutdown(void)
{
    receiveEventServerShutdown(FEAL_OK, FEAL_INVALID_HANDLE, -1);
}

void feal::StreamGeneric::connected_to_server(feal::handle_t fd)
{
    receiveEventConnectedToServer(FEAL_OK, fd, -1);
}

void feal::StreamGeneric::connection_read_avail(void)
{
    receiveEventReadAvail(FEAL_OK, sockfd, datareadavaillen(sockfd));
}

void feal::StreamGeneric::connection_write_avail(void)
{
    receiveEventWriteAvail(FEAL_OK, sockfd, -1);
}

void feal::StreamGeneric::connection_shutdown(void)
{
    receiveEventConnectionShutdown(FEAL_OK, FEAL_INVALID_HANDLE, -1);
}

void feal::StreamGeneric::receiveEventIncomingConn(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::StreamGeneric::receiveEventReadAvail(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::StreamGeneric::receiveEventWriteAvail(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::StreamGeneric::receiveEventClientShutdown(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::StreamGeneric::receiveEventServerShutdown(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::StreamGeneric::receiveEventConnectedToServer(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::StreamGeneric::receiveEventConnectionShutdown(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}
