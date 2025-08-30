//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

#if defined(_WIN32)
#define BUFCAST(x) (char *)x
#define CLOSESOCKET(x) closesocket(x)

#else
#define BUFCAST(x) (void *)x
#define CLOSESOCKET(x) close(x)

#endif

void feal::StreamGeneric::shutdownTool(void)
{
    disconnectAndReset();
}

feal::errenum_t feal::StreamGeneric::connect(handle_t fd, const sockaddr_all_t *addr,
                                             socklen_t addrlen)
{
    errenum_t res = FEAL_OK;
    if (addr == nullptr)
        return res;
    sockfd = fd;
    setNonBlocking(fd);
    int ret = ::connect(sockfd, &(addr->sa), addrlen);
    if ((ret == FEAL_HANDLE_ERROR) && (FEAL_GETHANDLEERRNO != FEAL_STREAM_EINPROGRESS))
    {
        res = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
        return res;
    }
    else if ((ret == FEAL_HANDLE_ERROR) && (FEAL_GETHANDLEERRNO == FEAL_STREAM_EINPROGRESS))
    {
        FEALDEBUGLOG("do_connect_in_progress");
        doConnectInProgress();
    }
    else if (ret == 0)
    {
        FEALDEBUGLOG("do_connect_ok");
        doConnectOk();
    }
    connectThread = std::thread(&connectLoopLauncher, this);
    return res;
}

feal::errenum_t feal::StreamGeneric::listen(handle_t fd, int backlog)
{
    errenum_t res = FEAL_OK;
    sockfd = fd;
    setNonBlocking(fd);
    if (serverThread.joinable())
        return res;
    if (::listen(sockfd, backlog) == FEAL_HANDLE_ERROR)
        res = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
    else
    {
        serverThread = std::thread(&serverLoopLauncher, this);
    }
    return res;
}

feal::errenum_t feal::StreamGeneric::recv(void *buf, uint32_t len, int32_t *bytes,
                                          feal::handle_t fd)
{
    errenum_t res = FEAL_OK;
    if (fd == FEAL_INVALID_HANDLE)
        fd = sockfd;
    ssize_t numbytes = ::recv(fd, BUFCAST(buf), (size_t)len, MSG_DONTWAIT);
#if defined(_WIN32)
    do_client_read_start(fd);
#endif
    if (numbytes == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (bytes)
        *bytes = (int32_t)numbytes;
    return res;
}

feal::errenum_t feal::StreamGeneric::send(void *buf, uint32_t len, int32_t *bytes,
                                          feal::handle_t fd)
{
    errenum_t res = FEAL_OK;
    if (fd == FEAL_INVALID_HANDLE)
        fd = sockfd;
    ssize_t numbytes = ::send(fd, BUFCAST(buf), (size_t)len, MSG_DONTWAIT);
    if (numbytes == FEAL_HANDLE_ERROR)
    {
        if ((FEAL_GETHANDLEERRNO == EAGAIN) || (FEAL_GETHANDLEERRNO == EWOULDBLOCK))
        {
            doSendAvailNotify(fd);
        }
        res = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (bytes)
        *bytes = (int32_t)numbytes;
    return res;
}

feal::errenum_t feal::StreamGeneric::disconnectClient(feal::handle_t client_sockfd)
{
    errenum_t res = FEAL_OK;
    mapReaders.erase(client_sockfd);
    if (doClientShutdown(client_sockfd) == -1)
        res = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
    return res;
}

feal::errenum_t feal::StreamGeneric::disconnectAndReset(void)
{
    for (auto it = mapReaders.begin(); it != mapReaders.end(); ++it)
        doClientShutdown(it->first);
    mapReaders.clear();
    errenum_t res = FEAL_OK;
    if (doFullShutdown() == -1)
        res = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
    if (serverThread.joinable())
        serverThread.join();
    if (connectThread.joinable())
        connectThread.join();
    return res;
}

void feal::StreamGeneric::serverLoopLauncher(StreamGeneric *p)
{
    if (p)
        p->serverLoop();
}

void feal::StreamGeneric::connectLoopLauncher(StreamGeneric *p)
{
    if (p)
        p->connectLoop();
}

int feal::StreamGeneric::acceptNewConn(void)
{
    sockaddr_all_t sall;
    memset(&sall, 0, sizeof(sall));
    socklen_t socklength = sizeof(sall);
    handle_t sock_conn_fd = accept(sockfd, &(sall.sa), &socklength);
    errenum_t errnum = FEAL_OK;
    if (sock_conn_fd == FEAL_INVALID_HANDLE)
    {
        errnum = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
    }
    receiveEventIncomingConn(errnum, sock_conn_fd, -1);
    return sock_conn_fd;
}

void feal::StreamGeneric::clientReadAvail(feal::handle_t client_sockfd)
{
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        clientst_t cst = it->second;
        if (cst.wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = cst.wkact.lock();
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(cst.evtclientreadavail));
            itw.get()->errnum = FEAL_OK;
            itw.get()->fd = client_sockfd;
            itw.get()->datalen = datareadavaillen(client_sockfd);
            act.get()->receiveEvent(itw);
        }
    }
}

void feal::StreamGeneric::clientWriteAvail(feal::handle_t client_sockfd)
{
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        clientst_t cst = it->second;
        if (cst.wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = cst.wkact.lock();
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(cst.evtclientwriteavail));
            itw.get()->errnum = FEAL_OK;
            itw.get()->fd = client_sockfd;
            itw.get()->datalen = -1;
            act.get()->receiveEvent(itw);
        }
    }
}

void feal::StreamGeneric::clientShutdown(feal::handle_t client_sockfd)
{
    auto it = mapReaders.find(client_sockfd);
    if (it != mapReaders.end())
    {
        clientst_t cst = it->second;
        if (cst.wkact.expired() == false)
        {
            std::shared_ptr<Actor> act = cst.wkact.lock();
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(cst.evtclientshutdown));
            itw.get()->errnum = FEAL_OK;
            itw.get()->fd = client_sockfd;
            itw.get()->datalen = -1;
            act.get()->receiveEvent(itw);
        }
    }
    mapReaders.erase(client_sockfd);
}

void feal::StreamGeneric::serverShutdown(void)
{
    receiveEventServerShutdown(FEAL_OK, FEAL_INVALID_HANDLE, -1);
}

void feal::StreamGeneric::connectedToServer(feal::handle_t fd)
{
    receiveEventConnectedToServer(FEAL_OK, fd, -1);
}

void feal::StreamGeneric::connectionReadAvail(void)
{
    receiveEventReadAvail(FEAL_OK, sockfd, datareadavaillen(sockfd));
}

void feal::StreamGeneric::connectionWriteAvail(void)
{
    receiveEventWriteAvail(FEAL_OK, sockfd, -1);
}

void feal::StreamGeneric::connectionShutdown(void)
{
    receiveEventConnectionShutdown(FEAL_OK, FEAL_INVALID_HANDLE, -1);
}

void feal::StreamGeneric::receiveEventIncomingConn(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}

void feal::StreamGeneric::receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}

void feal::StreamGeneric::receiveEventWriteAvail(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}

void feal::StreamGeneric::receiveEventClientShutdown(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}

void feal::StreamGeneric::receiveEventServerShutdown(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}

void feal::StreamGeneric::receiveEventConnectedToServer(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}

void feal::StreamGeneric::receiveEventConnectionShutdown(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}
