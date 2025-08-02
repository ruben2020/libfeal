//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include <stdlib.h>

#include "ActorA.h"

#if defined (_WIN32)
#define FILETOMONITOR1 "C:\\Users\\AppData\\Local\\Temp\\test_filedirmon1.txt"
#define FILETOMONITOR2 "C:\\Users\\AppData\\Local\\Temp\\test_filedirmon2.txt"
#else
#define FILETOMONITOR1 "/tmp/test_filedirmon1.txt"
#define FILETOMONITOR2 "/tmp/test_filedirmon2.txt"
#endif

void ActorA::initActor(void)
{
    printf("ActorA::initActor\n");
    timers.init(this);
    fdmon.init(this);
    fdmon.subscribeReadAvail<EvtFDMReadAvail>();
    fdmon.subscribeDescErr<EvtFDMErr>();
}

void ActorA::startActor(void)
{
    printf("ActorA::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(20));
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
    FILE *fp = fopen(FILETOMONITOR1, "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", n++);
        fclose(fp);
    }
    fp = fopen(FILETOMONITOR2, "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", 99 + n);
        fclose(fp);
    }
    fdmon.start_monitoring();
    fdmon.add(FILETOMONITOR1, FEAL_FDM_CLOSE_WRITE, &wnum1);
    fdmon.add(FILETOMONITOR2, FEAL_FDM_CLOSE_WRITE, &wnum2);
}

void ActorA::pauseActor(void)
{
    printf("ActorA::pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("ActorA shutdown complete\n");
}

void ActorA::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtDelayTimer>();
    fdmon.remove(wnum1);
    fdmon.remove(wnum2);
    fdmon.close_and_reset();
    shutdown();
}

void ActorA::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtDelayTimer\n");
    FILE *fp = fopen(FILETOMONITOR1, "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", n++);
        fclose(fp);
    }
    fp = fopen(FILETOMONITOR2, "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", 99 + n);
        fclose(fp);
    }
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void ActorA::handleEvent(std::shared_ptr<EvtFDMReadAvail> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtFDMReadAvail\n");
    std::string fn;
    if      (pevt->fd == wnum1) fn = FILETOMONITOR1;
    else if (pevt->fd == wnum2) fn = FILETOMONITOR2;
    else fn = "Unknown file";
    printf("filedirmon event for file %s for event %s\n", 
        fn.c_str(),
        (pevt->flags && FEAL_FDM_CLOSE_WRITE == FEAL_FDM_CLOSE_WRITE ? "FEAL_FDM_CLOSE_WRITE" : "something else"));
    printf("File contents of %s: ", fn.c_str());
    FILE *fp = fopen(fn.c_str(), "r");
    if (fp != NULL)
    {
        int val=0;
        fscanf(fp, "%d\n", &val);
        printf("%d\n", val);
        fclose(fp);
    }
}

void ActorA::handleEvent(std::shared_ptr<EvtFDMErr> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtFDMErr\n");
    timers.stopTimer<EvtDelayTimer>();
    fdmon.close_and_reset();
}

