#include "feal.h"

feal::Event::~Event()
{
    delete sender;
    sender = nullptr;
}

feal::EventId_t feal::Event::getId(void)
{
    return 0; // which is invalid
}

feal::EventId_t feal::Event::generateUniqueID(void)
{
    static EventId_t uid = 0;
    ++uid;
    return uid;
}

void feal::Event::replyEvent(Event* pevt)
{
    if ((sender != nullptr)&&(pevt))
    {
        if (*sender)
        {
            std::shared_ptr<Event> spevt(pevt);
            (**sender).receiveEvent(spevt);
        }
    }
}

void feal::Event::setSender(feal::Actor* act)
{
    //sender = new std::shared_ptr<Actor>(act);
}
