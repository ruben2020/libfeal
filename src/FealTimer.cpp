/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Licensed under the Apache License, Version 2.0
 * with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You can find a copy of the License in the LICENSE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 * 
 */
 
#include <cstdio>
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
    std::unique_lock<std::mutex> ulk(mtxTimer);
    while (timerValid)
    {
        if (timerActive && timerValid)
        {
            mtxTimerVar.lock();
            std::chrono::time_point<std::chrono::steady_clock> tp = tpoint;
            mtxTimerVar.unlock();
            cvTimer.wait_until(ulk, tp);
            if (timerActive && timerValid)
            {
                if (timerRepeat)
                {
                    mtxTimerVar.lock();
                    tpoint += secs;
                    mtxTimerVar.unlock();
                }
                else timerActive = false;
                timerEvent.get()->replyEvent(EventBus::getInstance().cloneEvent(timerEvent));
            }
        }
        else if (timerValid)
        {
            timerDormant = true;
            cvTimer.wait(ulk);
        }
    }
}

void feal::Timer::stopTimer(void)
{
    timerActive = false;
    timerRepeat = false;
    timerDormant = false;
    cvTimer.notify_all();
    do
    {
#if defined (_WIN32)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
#else
        std::this_thread::sleep_for(std::chrono::microseconds(50));
#endif
    } while (!timerDormant);
}

std::chrono::seconds feal::Timer::getTimerRepeat(void)
{
    std::chrono::seconds secs1;
    mtxTimerVar.lock();
    secs1 = secs;
    mtxTimerVar.unlock();
    return secs1;
}

void feal::Timer::finalizeTimer(void)
{
    timerActive = false;
    timerRepeat = false;
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


