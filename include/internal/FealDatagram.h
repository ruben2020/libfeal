//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _FEAL_DATAGRAM_H
#define _FEAL_DATAGRAM_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{


class DatagramGeneric : public Tool
{
public:
DatagramGeneric() = default;
DatagramGeneric( const DatagramGeneric & ) = default;
DatagramGeneric& operator= ( const DatagramGeneric & ) = default;
~DatagramGeneric() = default;

void shutdownTool(void);

errenum monitor_sock(handle_t fd);
errenum recvfrom(void *buf, uint32_t len, int32_t* bytes, 
        sockaddr_all* addr,
        socklen_t* addrlen);
errenum sendto(void *buf, uint32_t len, 
    int32_t* bytes, const sockaddr_all* dest, 
    socklen_t destlen, bool confirm = false);
errenum close_and_reset(void);


protected:

std::thread datagramThread;
handle_t sockfd = FEAL_INVALID_HANDLE;

#if defined (_WIN32)
#define FEALDGRAM_MAXEVENTS  (FD_SETSIZE > 64 ? 64 : FD_SETSIZE)
handle_t sockread[FEALDGRAM_MAXEVENTS];
handle_t sockwrite[FEALDGRAM_MAXEVENTS];

#elif defined (__linux__)
#define FEALDGRAM_MAXEVENTS 64
int epfd = -1;

#else
#define FEALDGRAM_MAXEVENTS 64
int kq = -1;
#endif

virtual void receiveEventReadAvail(errenum errnum, handle_t fd, int datalen);
virtual void receiveEventWriteAvail(errenum errnum, handle_t fd, int datalen);
virtual void receiveEventSockErr(errenum errnum, handle_t fd, int datalen);

private:


static void dgramLoopLauncher(DatagramGeneric *p);
void dgramLoop(void);
void do_send_avail_notify(void);
void close_sock(void);
void sock_error(void);
void sock_read_avail(void);
void sock_write_avail(void);

};


template<typename Y>
class Datagram : public DatagramGeneric
{
public:
Datagram() = default;
Datagram( const Datagram & ) = default;
Datagram& operator= ( const Datagram & ) = default;
~Datagram() = default;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
}

template<typename T>
void subscribeReadAvail()
{
    T inst;
    actorptr->addEvent(actorptr, inst);
    evtread = std::make_shared<T>();
    EventBus::getInstance().registerEventCloner<T>();
}

template<typename T>
void subscribeWriteAvail()
{
    T inst;
    actorptr->addEvent(actorptr, inst);
    evtwrite = std::make_shared<T>();
    EventBus::getInstance().registerEventCloner<T>();
}

template<typename T>
void subscribeSockErr()
{
    T inst;
    actorptr->addEvent(actorptr, inst);
    evterrsock = std::make_shared<T>();
    EventBus::getInstance().registerEventCloner<T>();
}


protected:

void receiveEventReadAvail(errenum errnum, handle_t fd, int datalen)
{
    if (evtread.get())
    {
        auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(evtread));
        itw.get()->errnum = errnum;
        itw.get()->fd = fd;
        itw.get()->datalen = datalen;
        if (actorptr) actorptr->receiveEvent(itw);
    }
    else printf("No subscription using Datagram::subscribeReadAvail\n");
}

void receiveEventWriteAvail(errenum errnum, handle_t fd, int datalen)
{
    if (evtwrite.get())
    {
        auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(evtwrite));
        itw.get()->errnum = errnum;
        itw.get()->fd = fd;
        itw.get()->datalen = datalen;
        if (actorptr) actorptr->receiveEvent(itw);
    }
    else printf("No subscription using Datagram::subscribeWriteAvail\n");
}

void receiveEventSockErr(errenum errnum, handle_t fd, int datalen)
{
    if (evterrsock.get())
    {
        auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(evterrsock));
        itw.get()->errnum = errnum;
        itw.get()->fd = fd;
        itw.get()->datalen = datalen;
        if (actorptr) actorptr->receiveEvent(itw);
    }
    else printf("No subscription using Datagram::subscribeSockErr\n");
}


private:

Y* actorptr = nullptr;
std::shared_ptr<EventComm> evtread;
std::shared_ptr<EventComm> evtwrite;
std::shared_ptr<EventComm> evterrsock;

};


} // namespace feal

#endif // _FEAL_DATAGRAM_H
