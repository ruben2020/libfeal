//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_DESCMON_H
#define FEAL_DESCMON_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{

class DescMonGeneric : public Tool
{
   public:
    DescMonGeneric() = default;
    DescMonGeneric(const DescMonGeneric&) = default;
    DescMonGeneric& operator=(const DescMonGeneric&) = default;
    ~DescMonGeneric() override = default;

    void shutdownTool(void) override;

    errenum_t startMonitoring(void);
    errenum_t add(handle_t fd);
    errenum_t remove(handle_t fd);
    errenum_t closeAndReset(void);

   protected:
    std::thread DescMonThread;

#if defined(_WIN32)
#define FEALDESCMON_MAXEVENTS (FD_SETSIZE > 64 ? 64 : FD_SETSIZE)
    handle_t sockread[FEALDESCMON_MAXEVENTS];
    handle_t sockwrite[FEALDESCMON_MAXEVENTS];
    std::atomic_bool active = true;

#elif defined(__linux__)
#define FEALDESCMON_MAXEVENTS 64
    int epfd = -1;

#else
#define FEALDESCMON_MAXEVENTS 64
    int kq = -1;
#endif

    void init(void);

    virtual void receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventWriteAvail(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventDescErr(errenum_t errnum, handle_t fd, int datalen);

   private:
    static void fdmonLoopLauncher(DescMonGeneric* p);
    void fdmonLoop(void);
    void fdError(handle_t fd);
    void fdReadAvail(handle_t fd);
    void fdWriteAvail(handle_t fd);
};

template <typename Y>
class DescMon : public DescMonGeneric
{
   public:
    DescMon() = default;
    DescMon(const DescMon&) = default;
    DescMon& operator=(const DescMon&) = default;
    ~DescMon() override = default;

    void init(Y* p)
    {
        actorptr = p;
        p->addTool(this);
        DescMonGeneric::init();
    }

    template <typename T>
    void subscribeReadAvail()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtread = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    template <typename T>
    void subscribeWriteAvail()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtwrite = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    template <typename T>
    void subscribeDescErr()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evterrfd = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

   protected:
    void receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evtread.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtread));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using DescMon::subscribeReadAvail\n");
    }

    void receiveEventWriteAvail(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evtwrite.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtwrite));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using DescMon::subscribeWriteAvail\n");
    }

    void receiveEventDescErr(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evterrfd.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evterrfd));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using DescMon::subscribeDescErr\n");
    }

   private:
    Y* actorptr = nullptr;
    std::shared_ptr<EventComm> evtread;
    std::shared_ptr<EventComm> evtwrite;
    std::shared_ptr<EventComm> evterrfd;
};

}  // namespace feal

#endif  // FEAL_FILEDESCMON_H
