//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _FEAL_FILEDIRMON_H
#define _FEAL_FILEDIRMON_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{


class FileDirMonGeneric : public Tool
{
public:
FileDirMonGeneric() = default;
FileDirMonGeneric( const FileDirMonGeneric & ) = default;
FileDirMonGeneric& operator= ( const FileDirMonGeneric & ) = default;
~FileDirMonGeneric() = default;

void shutdownTool(void);

errenum monitor(const char *s, mask_t mask);
errenum close_and_reset(void);

protected:

std::thread FileDirMonThread;
handle_t genfd = FEAL_INVALID_HANDLE;
handle_t genwd = FEAL_INVALID_HANDLE;
mask_t fdmask = 0;
std::string fdname;

#if defined (__linux__)
const unsigned int max_events = 64;
int epfd = -1;

#else
const unsigned int max_events = 64;
int kq = -1;
#endif

virtual void receiveEventReadAvail(errenum errnum, handle_t fd, int datalen, mask_t mask1);
virtual void receiveEventDescErr(errenum errnum, handle_t fd, int datalen, mask_t mask1);

private:

static void fdmonLoopLauncher(FileDirMonGeneric *p);
void fdmonLoop(void);
void close_fd(void);
void fd_error(void);
void fd_read_avail(mask_t mask1);
#if defined (__linux__)
static int epoll_ctl_add(int epfd, handle_t fd, uint32_t events);
static int epoll_ctl_mod(int epfd, handle_t fd, uint32_t events);
#endif


};


template<typename Y>
class FileDirMon : public FileDirMonGeneric
{
public:
FileDirMon() = default;
FileDirMon( const FileDirMon & ) = default;
FileDirMon& operator= ( const FileDirMon & ) = default;
~FileDirMon() = default;

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
void subscribeDescErr()
{
    T inst;
    actorptr->addEvent(actorptr, inst);
    evterrfd = std::make_shared<T>();
    EventBus::getInstance().registerEventCloner<T>();
}


protected:

void receiveEventReadAvail(errenum errnum, handle_t fd, int datalen, mask_t mask1)
{
    if (evtread.get())
    {
        auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(evtread));
        itw.get()->errnum = errnum;
        itw.get()->fd = fd;
        itw.get()->datalen = datalen;
        itw.get()->mask = mask1;
        if (actorptr) actorptr->receiveEvent(itw);
    }
    else printf("No subscription using FileDirMon::subscribeReadAvail\n");
}

void receiveEventDescErr(errenum errnum, handle_t fd, int datalen, mask_t mask1)
{
    if (evterrfd.get())
    {
        auto itw = std::dynamic_pointer_cast<EventComm>(EventBus::getInstance().cloneEvent(evterrfd));
        itw.get()->errnum = errnum;
        itw.get()->fd = fd;
        itw.get()->datalen = datalen;
        itw.get()->mask = mask1;
        if (actorptr) actorptr->receiveEvent(itw);
    }
    else printf("No subscription using FileDirMon::subscribeDescErr\n");
}


private:

Y* actorptr = nullptr;
std::shared_ptr<EventComm> evtread;
std::shared_ptr<EventComm> evterrfd;

};


} // namespace feal

#endif // _FEAL_FILEDESCMON_H
