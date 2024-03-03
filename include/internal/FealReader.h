//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _FEAL_READER_H
#define _FEAL_READER_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{


class ReaderGeneric : public Tool
{
public:
ReaderGeneric() = default;
ReaderGeneric( const ReaderGeneric & ) = default;
ReaderGeneric& operator= ( const ReaderGeneric & ) = default;
~ReaderGeneric() = default;

void shutdownTool(void);

errenum close_and_reset(void);

protected:

std::thread readerThread;
handle_t readerfd = FEAL_INVALID_HANDLE;
errenum open_pipe_for_reading(const char *pathname);
errenum registerhandle(handle_t fd);

#if defined (__linux__)
const unsigned int max_events = 64;
int epfd = -1;

#else
const unsigned int max_events = 64;
int kq = -1;
#endif

virtual void receiveEventReadAvail(errenum errnum, handle_t fd, int datalen);
virtual void receiveEventSockErr(errenum errnum, handle_t fd, int datalen);

private:

static void readerLoopLauncher(ReaderGeneric *p);
void readerLoop(void);
void close_handle(void);
void handle_error(void);
void handle_read_avail(void);

#if defined (__linux__)
static int epoll_ctl_add(int epfd, handle_t fd, uint32_t events);
static int epoll_ctl_mod(int epfd, handle_t fd, uint32_t events);
#endif


};


template<typename Y>
class Reader : public ReaderGeneric
{
public:
Reader() = default;
Reader( const Reader & ) = default;
Reader& operator= ( const Reader & ) = default;
~Reader() = default;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
}


template<typename T>
errenum subscribeReadAvail(handle_t fd)
{
    T inst;
    actorptr->addEvent(actorptr, inst);
    evtread = std::make_shared<T>();
    EventBus::getInstance().registerEventCloner<T>();
    return ReaderGeneric::registerhandle(fd);
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
    auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(evtread));
    itw.get()->errnum = errnum;
    itw.get()->fd = fd;
    itw.get()->datalen = datalen;
    if (actorptr) actorptr->receiveEvent(itw);
}

void receiveEventSockErr(errenum errnum, handle_t fd, int datalen)
{
    auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(evterrsock));
    itw.get()->errnum = errnum;
    itw.get()->fd = fd;
    itw.get()->datalen = datalen;
    if (actorptr) actorptr->receiveEvent(itw);
}

private:
Y* actorptr = nullptr;
std::shared_ptr<EventComm> evtread;
std::shared_ptr<EventComm> evterrsock;

};

} // namespace feal

#endif // _FEAL_READER_H
