#include "feal.h"

feal::Timer::~Timer()
{
    finalizeTimer();
}

void feal::Timer::initTimer(void)
{
    timerActive = false;
    timerValid = true;
    timerThread = std::thread(&timerLoopLauncher, this);
}

void feal::Timer::timerLoopLauncher(feal::Timer *p)
{
    if (p) p->timerLoop();
}

void feal::Timer::timerLoop(void)
{
    while (timerValid)
    {
        if (timerActive && timerValid)
        {
            std::unique_lock<std::mutex> ulk1(mtxTimer);
            mtxTimerVar.lock();
            std::chrono::time_point<std::chrono::steady_clock> tp = tpoint;
            mtxTimerVar.unlock();
            cvTimer.wait_until(ulk1, tp);
            if (timerActive && timerValid)
            {
                if (timerPeriodic)
                {
                    mtxTimerVar.lock();
                    tpoint += secs;
                    mtxTimerVar.unlock();
                }
                else timerActive = false;
                timerEvent.get()->replyEvent(timerEvent);
            }
        }
        else if (timerValid)
        {
            timerDormant = true;
            std::unique_lock<std::mutex> ulk2(mtxTimer);
            cvTimer.wait(ulk2);
        }
    }
}

void feal::Timer::stopTimer(void)
{
    timerActive = false;
    timerPeriodic = false;
    timerDormant = false;
    cvTimer.notify_all();
    do
    {
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    } while (!timerDormant);
}

void feal::Timer::finalizeTimer(void)
{
    timerActive = false;
    timerPeriodic = false;
    timerDormant = false;
    timerValid = false;
    cvTimer.notify_all();
    if (timerThread.joinable())
    {
        timerThread.join();
    }
    timerEvent.reset();
}

void feal::Timer::setTimerEvent(std::shared_ptr<Event> timEvt)
{
    timerEvent = timEvt;
}


