//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_BASESTREAM_H
#define FEAL_BASESTREAM_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined(_WIN32)
#include <atomic>
#elif defined(__linux__)
#include <sys/epoll.h>
#else
#include <unistd.h>
#include <sys/event.h>
#include <cstring>
#endif

namespace feal
{

class BaseStream : public Tool
{
   public:
    BaseStream() = default;
    BaseStream(const BaseStream&) = default;
    BaseStream& operator=(const BaseStream&) = default;
    ~BaseStream() override = default;

   protected:
    handle_t sockfd = FEAL_INVALID_HANDLE;
    bool waitingforconn = false;

    virtual int acceptNewConn(void);
    virtual void clientReadAvail(handle_t client_sockfd);
    virtual void clientWriteAvail(handle_t client_sockfd);
    virtual void clientShutdown(handle_t client_sockfd);
    virtual void serverShutdown(void);
    virtual void connectedToServer(handle_t fd);
    virtual void connectionReadAvail(void);
    virtual void connectionWriteAvail(void);
    virtual void connectionShutdown(void);

    void serverLoop(void);

    int doClientReadStart(handle_t client_sockfd);
    int doClientShutdown(handle_t client_sockfd);
    int doFullShutdown(void);
    void doConnectInProgress(void);
    void doConnectOk(void);
    void doSendAvailNotify(handle_t fd);

    void connectLoop(void);

   private:
#if defined(_WIN32)
#define FEALBASESTREAM_MAXEVENTS (FD_SETSIZE > 64 ? 64 : FD_SETSIZE)
    handle_t sockread[FEALBASESTREAM_MAXEVENTS];
    handle_t sockwrite[FEALBASESTREAM_MAXEVENTS];
    handle_t sockexcpt[FEALBASESTREAM_MAXEVENTS];

#elif defined(__linux__)
#define FEALBASESTREAM_MAXEVENTS 64
    int epfd = -1;
#else

#define FEALBASESTREAM_MAXEVENTS 64
    int kq = -1;
#endif
};

}  // namespace feal

#endif  // FEAL_BASESTREAM_H
