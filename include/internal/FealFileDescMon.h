//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _FEAL_FILEDESCMON_H
#define _FEAL_FILEDESCMON_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{


class FileDescMonGeneric : public Tool
{
public:
FileDescMonGeneric() = default;
FileDescMonGeneric( const FileDescMonGeneric & ) = default;
FileDescMonGeneric& operator= ( const FileDescMonGeneric & ) = default;
~FileDescMonGeneric() = default;

void shutdownTool(void);

errenum monitor(handle_t fd);
errenum close_and_reset(void);

protected:

std::thread FileDescMonThread;
handle_t genfd = FEAL_INVALID_HANDLE;

#if defined (__linux__)
const unsigned int max_events = 64;
int epfd = -1;

#else
const unsigned int max_events = 64;
int kq = -1;
#endif

virtual void receiveEventReadAvail(errenum errnum, handle_t fd, int datalen);
virtual void receiveEventWriteAvail(errenum errnum, handle_t fd, int datalen);
virtual void receiveEventFDErr(errenum errnum, handle_t fd, int datalen);

private:

static void fdmonLoopLauncher(FileDescMonGeneric *p);
void fdmonLoop(void);
void close_fd(void);
void fd_error(void);
void fd_read_avail(void);
void fd_write_avail(void);
#if defined (__linux__)
static int epoll_ctl_add(int epfd, handle_t fd, uint32_t events);
static int epoll_ctl_mod(int epfd, handle_t fd, uint32_t events);
#endif


};


template<typename Y>
class FileDescMon : public FileDescMonGeneric
{
public:
FileDescMon() = default;
FileDescMon( const FileDescMon & ) = default;
FileDescMon& operator= ( const FileDescMon & ) = default;
~FileDescMon() = default;

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
void subscribeFDErr()
{
    T inst;
    actorptr->addEvent(actorptr, inst);
    evterrfd = std::make_shared<T>();
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
    else printf("No subscription using FileDescMon::subscribeReadAvail\n");
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
    else printf("No subscription using FileDescMon::subscribeWriteAvail\n");
}

void receiveEventFDErr(errenum errnum, handle_t fd, int datalen)
{
    if (evterrfd.get())
    {
        auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(evterrfd));
        itw.get()->errnum = errnum;
        itw.get()->fd = fd;
        itw.get()->datalen = datalen;
        if (actorptr) actorptr->receiveEvent(itw);
    }
    else printf("No subscription using FileDescMon::subscribeFDErr\n");
}


private:

Y* actorptr = nullptr;
std::shared_ptr<EventComm> evtread;
std::shared_ptr<EventComm> evtwrite;
std::shared_ptr<EventComm> evterrfd;

};


} // namespace feal

#endif // _FEAL_FILEDESCMON_H
