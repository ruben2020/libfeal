//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#ifndef _FEAL_PIPEREADER_H
#define _FEAL_PIPEREADER_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{

template<typename Y>
class PipeReader : public ReaderGeneric
{
public:
PipeReader() = default;
PipeReader( const PipeReader & ) = default;
PipeReader& operator= ( const PipeReader & ) = default;
~PipeReader() = default;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
}

template<typename T>
errenum subscribe_and_open(const char *pathname)
{
    handle_t fd;
    close_and_reset();
    fd = open(pathname, O_RDONLY | O_NONBLOCK);
    if (fd == -1) return static_cast<errenum>(errno);
    T inst;
    actorptr->addEvent(actorptr, inst);
    myevt = std::make_shared<T>();
    EventBus::getInstance().registerEventCloner<T>();
    return ReaderGeneric::registerhandle(fd);
}

protected:

void receiveEvent(handle_t fd, bool error, int datalen)
{
    auto itw = std::dynamic_pointer_cast<EvtReader>(EventBus::getInstance().cloneEvent(myevt));
    itw.get()->readerfd = fd;
    itw.get()->error = error;
    itw.get()->datalen = datalen;
    if (actorptr) actorptr->receiveEvent(itw);
}

private:
Y* actorptr = nullptr;
std::shared_ptr<EvtReader> myevt;

};

} // namespace feal

#endif // _FEAL_PIPEREADER_H
