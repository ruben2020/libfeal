#include <memory>
#include "feal.h"


void feal::Actor::initActor(void){}
void feal::Actor::startActor(void){}
void feal::Actor::pauseActor(void){}
void feal::Actor::shutdownActor(void){}

void feal::Actor::init(void)
{
    if (threadValid == false) return; // not possible after shutdown
    initActor();
}

void feal::Actor::start(void)
{
    if (threadValid == false) return; // not possible after shutdown
    threadRunning = true;
    if (actorthread.joinable() == false)
    {
        // event loop is newly starting
        actorthread = std::thread(&eventLoop, this);
    }
    else
    {
        // event loop is resuming from pause
        cvEventLoop.notify_all();
    }
    startActor();
}

void feal::Actor::pause(void)
{
    if (threadValid == false) return; // not possible after shutdown
    pauseActor();
    // event loop is pausing
    threadRunning = false;
}

void feal::Actor::shutdown(void)
{
    shutdownActor();
    threadValid = false;
    threadRunning = false;
    cvEventLoop.notify_all();
    if (actorthread.joinable())
    {
        actorthread.join();
    }
    cvWaitShutdown.notify_all();
}

bool feal::Actor::isActive(void)
{
    return (threadValid && threadRunning);
}

void feal::Actor::wait_for_shutdown(void)
{
    std::unique_lock<std::mutex> ulk(mtxWaitShutdown);
    cvWaitShutdown.wait(ulk);           
}

void feal::Actor::eventLoop(feal::Actor* p)
{
    bool queue_empty;
    Event fe;
    EventId_t id;
    while (p->threadValid)
    {
        while (p->threadRunning)
        {
            p->mtxEventQueue.lock();
            queue_empty = p->evtQueue.empty();
            if (!queue_empty)
            {
                fe = p->evtQueue.front();
                p->evtQueue.pop();
            }
            p->mtxEventQueue.unlock();
            if (queue_empty) break;
            id = fe.getId();
            if (id == 0) continue;
            auto it = p->mapEventHandlers.find(id);
            if (it != p->mapEventHandlers.end())
            {
                it->second(fe);
            }
        }
        if (p->threadValid)
        {
            std::unique_lock<std::mutex> ulk(p->mtxEventLoop);
            p->cvEventLoop.wait(ulk);           
        }
    }
    
}

void feal::Actor::receiveEvent(feal::Event& evt)
{
    if (threadValid) // not possible after shutdown
    {
        Event evt2 = evt; // make a copy
        mtxEventQueue.lock();
        evtQueue.push(evt2);
        mtxEventQueue.unlock();
        cvEventLoop.notify_all();
    }
}


void feal::Actor::publishEvent(feal::Event& evt)
{
    evt.setSender(this);
    EventBus::getInstance().publishEvent(evt);
}



void feal::initAll(feal::actor_vec_t& vec)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).init();
    }
}

void feal::startAll(feal::actor_vec_t& vec)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).start();
    }
}

void feal::pauseAll(feal::actor_vec_t& vec)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).pause();
    }
}

void feal::shutdownAll(feal::actor_vec_t& vec)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).shutdown();
    }
}

void feal::receiveEventAll(feal::actor_vec_t& vec, feal::Event& evt)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).receiveEvent(evt);
    }
}

