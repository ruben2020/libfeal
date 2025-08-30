//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEALFIFOREADER_H
#define FEALFIFOREADER_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{

template <typename Y>
class FifoReader : public ReaderGeneric
{
   public:
    FifoReader() = default;
    FifoReader(const FifoReader&) = default;
    FifoReader& operator=(const FifoReader&) = default;
    ~FifoReader() override = default;

    void init(Y* p)
    {
        actorptr = p;
        p->addTool(this);
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
    void subscribeSockErr()
    {
        T inst;
        actorptr->addEvent(actorptr, inst);
        evterrsock = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
    }

    errenum_t open(const char* pathname)
    {
        handle_t fd;
        closeAndReset();
        fd = ::open(pathname, O_RDONLY | O_NONBLOCK);
        if (fd == -1)
            return static_cast<errenum_t>(errno);
        return ReaderGeneric::registerhandle(fd);
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
            printf("No subscription using FifoReader::subscribeReadAvail\n");
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
            printf("No subscription using FifoReader::subscribeSockErr\n");
    }

   private:
    Y* actorptr = nullptr;
    std::shared_ptr<EventComm> evtread;
    std::shared_ptr<EventComm> evterrsock;
};

}  // namespace feal

#endif  // FEALFIFOREADER_H
