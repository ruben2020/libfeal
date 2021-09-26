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
    std::shared_ptr<Event> p = std::make_shared<EventStartActor>();
    receiveEvent(p);
    if (actorThread.joinable() == false)
    {
        // event loop is newly starting
        actorThread = std::thread(&eventLoopLauncher, this);
    }
    else
    {
        // event loop is resuming from pause
        cvEventLoop.notify_all();
    }
    for (std::vector<feal::Tool*>::size_type i=0; i < vecTool.size(); i++)
        (vecTool[i])->startTool();
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
    std::shared_ptr<Event> p = std::make_shared<EventPauseActor>();
    receiveEvent(p);
    for (std::vector<feal::Tool*>::size_type i=0; i < vecTool.size(); i++)
        (vecTool[i])->pauseTool();
}

void feal::Actor::shutdown(void)
{
    shutdownActor();
    for (std::vector<feal::Tool*>::size_type i=0; i < vecTool.size(); i++)
        (vecTool[i])->shutdownTool();
    threadValid = false;
    threadRunning = false;
    cvEventLoop.notify_all();
    if ((actorThread.get_id() != std::this_thread::get_id()) &&
                        (actorThread.joinable()))
    {
        actorThread.join();
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
    if ((actorThread.get_id() != std::this_thread::get_id()) &&
                        (actorThread.joinable()))
    {
        actorThread.join();
    }
}

void feal::Actor::addTool(Tool* p)
{
    vecTool.push_back(p);
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
            if ((queue_empty) || (!threadValid)) break;
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

void feal::Actor::publishEvent(std::shared_ptr<feal::Event> pevt)
{
   if (pevt)
    {
        std::weak_ptr<Actor> wkact = shared_from_this();
        pevt.get()->setSender(wkact);
        EventBus::getInstance().publishEvent(pevt);
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
