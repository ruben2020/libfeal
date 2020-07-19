#include "feal.h"

feal::EventId_t feal::Event::getId(void)
{
    return 0; // which is invalid
}

feal::EventId_t feal::Event::generateUniqueID(void)
{
    static EventId_t uid = 0;
    return (++uid);
}

void feal::Event::replyEvent(feal::Event& evt)
{
    if (sender != nullptr)
    {
        if (*sender)
        {
            (**sender).receiveEvent(evt);
        }
    }
}

void feal::Event::setSender(feal::Actor* act)
{
    sender = new std::shared_ptr<Actor>(act);
}
