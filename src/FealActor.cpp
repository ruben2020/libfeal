#include <memory>
#include "feal.h"


void feal::Actor::initActor(void){}
void feal::Actor::startActor(void){}
void feal::Actor::pauseActor(void){}
void feal::Actor::shutdownActor(void){}

void feal::Actor::init(void)
{
    if (threadValid == false) return; // not possible after shutdown
    mapEventHandlers.insert(std::make_pair(EventStartActor::getIdOfType<EventStartActor>(),
            [this](std::shared_ptr<Event> fe)
                {   this->handleEvent(std::dynamic_pointer_cast<EventStartActor>(fe));  }
            )
    );
    mapEventHandlers.insert(std::make_pair(EventPauseActor::getIdOfType<EventPauseActor>(),
            [this](std::shared_ptr<Event> fe)
                {   this->handleEvent(std::dynamic_pointer_cast<EventPauseActor>(fe));  }
            )
    );
    initActor();
}   

void feal::Actor::handleEvent(std::shared_ptr<feal::EventStartActor> pevt)
{
    if (pevt) startActor();
}

void feal::Actor::start(void)
{
    if (threadValid == false) return; // not possible after shutdown
    threadRunning = true;
    if (actorthread.joinable() == false)
    {
        // event loop is newly starting
        actorthread = std::thread(&eventLoopLauncher, this);
    }
    else
    {
        // event loop is resuming from pause
        cvEventLoop.notify_all();
    }
    std::shared_ptr<Event> p((Event*) new EventStartActor());
    receiveEvent(p);
}

void feal::Actor::handleEvent(std::shared_ptr<feal::EventPauseActor> pevt)
{
    if (pevt)
    {
        pauseActor();
        // event loop is pausing
        threadRunning = false;
    }
}

void feal::Actor::pause(void)
{
    if (threadValid == false) return; // not possible after shutdown
    std::shared_ptr<Event> p((Event*) new EventPauseActor());
    receiveEvent(p);
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

void feal::Actor::eventLoopLauncher(feal::Actor* p)
{
    if (p) p->eventLoop();
}

void feal::Actor::eventLoop(void)
{
    bool queue_empty;
    std::shared_ptr<Event> fe;
    EventId_t id;
    while (threadValid)
    {
        while (threadRunning)
        {
            mtxEventQueue.lock();
            queue_empty = evtQueue.empty();
            if (!queue_empty)
            {
                fe = evtQueue.front();
                evtQueue.pop();
            }
            mtxEventQueue.unlock();
            if (queue_empty) break;
            id = fe.get()->getId();
            if (id == 0) continue;
            auto it = mapEventHandlers.find(id);
            if (it != mapEventHandlers.end())
            {
                it->second(fe);
            }
        }
        if (threadValid)
        {
            std::unique_lock<std::mutex> ulk(mtxEventLoop);
            cvEventLoop.wait(ulk);           
        }
    }
    
}

void feal::Actor::receiveEvent(std::shared_ptr<feal::Event> pevt)
{
    if ((threadValid) && (pevt)) // not possible after shutdown
    {
        mtxEventQueue.lock();
        evtQueue.push(pevt);
        mtxEventQueue.unlock();
        cvEventLoop.notify_all();
    }
}

void feal::Actor::publishEvent(Event* pevt)
{
    if (pevt)
    {
        pevt->setSender(this);
        std::shared_ptr<Event> spevt(pevt);
        EventBus::getInstance().publishEvent(spevt);
    }
}

void feal::Actor::publishEvent(std::shared_ptr<feal::Event> pevt)
{
   if (pevt)
    {
        pevt.get()->setSender(this);
        EventBus::getInstance().publishEvent(pevt);
    }
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

void feal::receiveEventAll(feal::actor_vec_t& vec, std::shared_ptr<feal::Event> pevt)
{
    if (!pevt) return;
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).receiveEvent(pevt);
    }
}

feal::EventId_t feal::EventStartActor::getId(void)
{
    return getIdOfType<EventStartActor>();
}

feal::EventId_t feal::EventPauseActor::getId(void)
{
    return getIdOfType<EventPauseActor>();
}
