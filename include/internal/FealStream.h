//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_STREAM_H
#define FEAL_STREAM_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined(_WIN32)
#define FEAL_STREAM_EINPROGRESS WSAEWOULDBLOCK
#else
#define FEAL_STREAM_EINPROGRESS EINPROGRESS
#endif

namespace feal
{

typedef struct
{
    std::weak_ptr<Actor> wkact;
    std::shared_ptr<EventComm> evtclientreadavail;
    std::shared_ptr<EventComm> evtclientwriteavail;
    std::shared_ptr<EventComm> evtclientshutdown;
} clientst_t;

class StreamGeneric : public BaseStream
{
   public:
    StreamGeneric() = default;
    StreamGeneric(const StreamGeneric&) = default;
    StreamGeneric& operator=(const StreamGeneric&) = default;
    ~StreamGeneric() override = default;

    void shutdownTool(void) override;

    errenum_t connect(handle_t fd, const sockaddr_all_t* addr, socklen_t addrlen);
    errenum_t listen(handle_t fd, int backlog = 32);
    errenum_t recv(void* buf, uint32_t len, int32_t* bytes, handle_t fd = FEAL_INVALID_HANDLE);
    errenum_t send(void* buf, uint32_t len, int32_t* bytes, handle_t fd = FEAL_INVALID_HANDLE);
    errenum_t disconnectClient(handle_t client_sockfd);
    errenum_t disconnectAndReset(void);

   protected:
    std::thread serverThread;
    std::thread connectThread;
    std::map<handle_t, clientst_t> mapReaders;

    static void serverLoopLauncher(StreamGeneric* p);
    static void connectLoopLauncher(StreamGeneric* p);

    int acceptNewConn(void) override;
    void clientReadAvail(handle_t client_sockfd) override;
    void clientWriteAvail(handle_t client_sockfd) override;
    void clientShutdown(handle_t client_sockfd) override;
    void serverShutdown(void) override;
    void connectedToServer(handle_t fd) override;
    void connectionReadAvail(void) override;
    void connectionWriteAvail(void) override;
    void connectionShutdown(void) override;

    virtual void receiveEventIncomingConn(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventWriteAvail(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventClientShutdown(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventServerShutdown(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventConnectedToServer(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventConnectionShutdown(errenum_t errnum, handle_t fd, int datalen);
};

template <typename Y>
class Stream : public StreamGeneric
{
   public:
    Stream() = default;
    Stream(const Stream&) = default;
    Stream& operator=(const Stream&) = default;
    ~Stream() override = default;

    void init(Y* p)
    {
        actorptr = p;
        p->addTool(this);
    }

    template <typename T>
    void subscribeIncomingConn()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtincomingconn = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    template <typename T>
    void subscribeReadAvail()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtreadavail = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    template <typename T>
    void subscribeWriteAvail()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtwriteavail = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    template <typename T>
    void subscribeServerShutdown()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtservershutdown = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    template <typename T>
    void subscribeConnectedToServer()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtconnectedtoserver = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    template <typename T>
    void subscribeConnectionShutdown()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtconnshutdown = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    template <class T, typename CRead, typename CWrite, typename CShutdown>
    errenum_t registerClient(T* p, handle_t client_sockfd)
    {
        errenum_t res = FEAL_OK;
        clientst_t cst;
        CRead instcr;
        CWrite instcw;
        CShutdown instcs;
        if (p)
        {
            cst.wkact = p->shared_from_this();
            p->addEvent(p, instcr);
            cst.evtclientreadavail = std::make_shared<CRead>();
            EventBus::getInstance().registerEventCloner<CRead>();
            p->addEvent(p, instcw);
            cst.evtclientwriteavail = std::make_shared<CWrite>();
            EventBus::getInstance().registerEventCloner<CWrite>();
            p->addEvent(p, instcs);
            cst.evtclientshutdown = std::make_shared<CShutdown>();
            EventBus::getInstance().registerEventCloner<CShutdown>();
        }
        auto it = mapReaders.find(client_sockfd);
        if (it == mapReaders.end())
        {
            mapReaders[client_sockfd] = cst;
            setNonBlocking(client_sockfd);
            if (doClientReadStart(client_sockfd) == -1)
                res = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
        }
        return res;
    }

   protected:
    void receiveEventIncomingConn(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evtincomingconn.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtincomingconn));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using Stream::subscribeIncomingConn\n");
    }

    void receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evtreadavail.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtreadavail));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using Stream::subscribeReadAvail\n");
    }

    void receiveEventWriteAvail(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evtwriteavail.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtwriteavail));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using Stream::subscribeWriteAvail\n");
    }

    void receiveEventServerShutdown(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evtservershutdown.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtservershutdown));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using Stream::subscribeServerShutdown\n");
    }

    void receiveEventConnectedToServer(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evtconnectedtoserver.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtconnectedtoserver));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using Stream::subscribeConnectedToServer\n");
    }

    void receiveEventConnectionShutdown(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evtconnshutdown.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtconnshutdown));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using Stream::subscribeConnectionShutdown\n");
    }

   private:
    Y* actorptr = nullptr;
    std::shared_ptr<EventComm> evtincomingconn;
    std::shared_ptr<EventComm> evtreadavail;
    std::shared_ptr<EventComm> evtwriteavail;
    std::shared_ptr<EventComm> evtservershutdown;
    std::shared_ptr<EventComm> evtconnectedtoserver;
    std::shared_ptr<EventComm> evtconnshutdown;
};

}  // namespace feal

#endif  // FEAL_STREAM_H
