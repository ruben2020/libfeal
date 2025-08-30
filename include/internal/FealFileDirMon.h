//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_FILEDIRMON_H
#define FEAL_FILEDIRMON_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{

class FileDirMonGeneric : public Tool
{
   public:
    FileDirMonGeneric() = default;
    FileDirMonGeneric(const FileDirMonGeneric&) = default;
    FileDirMonGeneric& operator=(const FileDirMonGeneric&) = default;
    ~FileDirMonGeneric() override = default;

    void shutdownTool(void) override;

    errenum_t startMonitoring(void);
    errenum_t add(const char* s, flags_t mask, handle_t* wnum);
    errenum_t remove(handle_t wnum);
    std::string getFilepath(handle_t wnum);
    errenum_t closeAndReset(void);

   protected:
    std::thread FileDirMonThread;

#if defined(__linux__)
#define FILEDIRMON_MAXEVENTS 64
    int epfd = -1;
    handle_t genfd = FEAL_INVALID_HANDLE;

#else
#define FILEDIRMON_MAXEVENTS 64
    int kq = -1;
#endif

    void init(void);

    virtual void receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen, flags_t flags1);
    virtual void receiveEventDescErr(errenum_t errnum, handle_t fd, int datalen, flags_t flags1);

   private:
    static void fdmonLoopLauncher(FileDirMonGeneric* p);
    void fdmonLoop(void);
    void fdError(void);
    void fdReadAvail(handle_t fd1, flags_t flags1);

    std::map<handle_t, std::string> fnmap;
};

template <typename Y>
class FileDirMon : public FileDirMonGeneric
{
   public:
    FileDirMon() = default;
    FileDirMon(const FileDirMon&) = default;
    FileDirMon& operator=(const FileDirMon&) = default;
    ~FileDirMon() override = default;

    void init(Y* p)
    {
        actorptr = p;
        p->addTool(this);
        FileDirMonGeneric::init();
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
    void subscribeDescErr()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evterrfd = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

   protected:
    void receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen, flags_t flags1) override
    {
        if (evtread.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evtread));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            itw.get()->flags = flags1;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using FileDirMon::subscribeReadAvail\n");
    }

    void receiveEventDescErr(errenum_t errnum, handle_t fd, int datalen, flags_t flags1) override
    {
        if (evterrfd.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evterrfd));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            itw.get()->flags = flags1;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using FileDirMon::subscribeDescErr\n");
    }

   private:
    Y* actorptr = nullptr;
    std::shared_ptr<EventComm> evtread;
    std::shared_ptr<EventComm> evterrfd;
};

}  // namespace feal

#endif  // FEAL_FILEDESCMON_H
