//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_READER_H
#define FEAL_READER_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{

class ReaderGeneric : public Tool
{
   public:
    ReaderGeneric() = default;
    ReaderGeneric(const ReaderGeneric&) = default;
    ReaderGeneric& operator=(const ReaderGeneric&) = default;
    ~ReaderGeneric() override = default;

    void shutdownTool(void) override;

    errenum_t closeAndReset(void);

   protected:
    std::thread readerThread;
    handle_t readerfd = FEAL_INVALID_HANDLE;
    errenum_t openPipeForReading(const char* pathname);
    errenum_t registerhandle(handle_t fd);

#if defined(__linux__)
#define FEALREADER_MAXEVENTS 64
    int epfd = -1;

#else
#define FEALREADER_MAXEVENTS 64
    int kq = -1;
#endif

    virtual void receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen);
    virtual void receiveEventSockErr(errenum_t errnum, handle_t fd, int datalen);

   private:
    static void readerLoopLauncher(ReaderGeneric* p);
    void readerLoop(void);
    void closeHandle(void);
    void handleError(void);
    void handleReadAvail(void);
};

template <typename Y>
class Reader : public ReaderGeneric
{
   public:
    Reader() = default;
    Reader(const Reader&) = default;
    Reader& operator=(const Reader&) = default;
    ~Reader() override = default;

    void init(Y* p)
    {
        actorptr = p;
        p->addTool(this);
    }

    template <typename T>
    errenum_t subscribeReadAvail(handle_t fd)
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evtread = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
        return ReaderGeneric::registerhandle(fd);
    }

    template <typename T>
    void subscribeSockErr()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evterrsock = std::make_shared<T>();
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
            printf("No subscription using Reader::subscribeReadAvail\n");
    }

    void receiveEventSockErr(errenum_t errnum, handle_t fd, int datalen) override
    {
        if (evterrsock.get())
        {
            auto itw = std::dynamic_pointer_cast<EventComm>(
                    EventBus::getInstance().cloneEvent(evterrsock));
            itw.get()->errnum = errnum;
            itw.get()->fd = fd;
            itw.get()->datalen = datalen;
            if (actorptr)
                actorptr->receiveEvent(itw);
        }
        else
            printf("No subscription using Reader::subscribeSockErr\n");
    }

   private:
    Y* actorptr = nullptr;
    std::shared_ptr<EventComm> evtread;
    std::shared_ptr<EventComm> evterrsock;
};

}  // namespace feal

#endif  // FEAL_READER_H
