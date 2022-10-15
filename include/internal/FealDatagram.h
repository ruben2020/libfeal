//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
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
EventId_t getId(void);
socket_t sockfd = FEAL_INVALID_SOCKET;
int datalen = -1;
};

class EvtDgramWriteAvail : public Event
{
public:
EvtDgramWriteAvail() = default;
EvtDgramWriteAvail( const EvtDgramWriteAvail & ) = default;
EvtDgramWriteAvail& operator= ( const EvtDgramWriteAvail & ) = default;
~EvtDgramWriteAvail() = default;
EventId_t getId(void);
socket_t sockfd = FEAL_INVALID_SOCKET;
};


class EvtSockErr : public Event
{
public:
EvtSockErr() = default;
EvtSockErr( const EvtSockErr & ) = default;
EvtSockErr& operator= ( const EvtSockErr & ) = default;
~EvtSockErr() = default;
EventId_t getId(void);
};

class DatagramGeneric : public Tool
{
public:
DatagramGeneric() = default;
DatagramGeneric( const DatagramGeneric & ) = default;
DatagramGeneric& operator= ( const DatagramGeneric & ) = default;
~DatagramGeneric() = default;

void shutdownTool(void);

errenum create_sock(family_t fam);
errenum bind_sock(feal::ipaddr* fa);
errenum recv_from(void *buf, 
    uint32_t len, int32_t* bytes, feal::ipaddr* src);
errenum send_to(void *buf, uint32_t len, int32_t* bytes, 
    feal::ipaddr* dest, bool confirm = false);

#if defined(unix) || defined(__unix__) || defined(__unix)
errenum bind_sock(struct sockaddr_un* su);
errenum recv_from(void *buf, 
    uint32_t len, int32_t* bytes,
    struct sockaddr_un* src, socklen_t *srcaddrlen);
errenum send_to(void *buf, uint32_t len, int32_t* bytes,
    struct sockaddr_un* dest, socklen_t destaddrlen,
    bool confirm = false);
#endif

errenum close_and_reset(void);

protected:

std::thread datagramThread;
socket_t sockfd = FEAL_INVALID_SOCKET;

#if defined (_WIN32)
#define FEALDGRAM_MAXEVENTS  (FD_SETSIZE > 64 ? 64 : FD_SETSIZE)
const int max_events = FEALDGRAM_MAXEVENTS;
socket_t sockread[FEALDGRAM_MAXEVENTS];
socket_t sockwrite[FEALDGRAM_MAXEVENTS];

#elif defined (__linux__)
const unsigned int max_events = 64;
int epfd = -1;

#else
const unsigned int max_events = 64;
int kq = -1;
#endif

virtual void add_events(void);
virtual void receiveEvent(std::shared_ptr<Event> pevt);

private:

static void dgramLoopLauncher(DatagramGeneric *p);
void dgramLoop(void);
void do_send_avail_notify(void);
void close_sock(void);
void sock_error(void);
void sock_read_avail(void);
void sock_write_avail(void);
#if defined (__linux__)
static int epoll_ctl_add(int epfd, socket_t fd, uint32_t events);
static int epoll_ctl_mod(int epfd, socket_t fd, uint32_t events);
#endif


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

protected:

void receiveEvent(std::shared_ptr<Event> pevt)
{
    if (actorptr) actorptr->receiveEvent(pevt);
}

void add_events(void)
{
    if (actorptr == nullptr) return;
    EvtSockErr ese;
    EvtDgramReadAvail edra;
    EvtDgramWriteAvail edwa;
    actorptr->addEvent(actorptr, ese);
    actorptr->addEvent(actorptr, edra);
    actorptr->addEvent(actorptr, edwa);
}

private:

Y* actorptr = nullptr;

};


} // namespace feal

#endif // _FEAL_DATAGRAM_H
