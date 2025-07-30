//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include <stdlib.h>

#include "ActorA.h"

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
    FILE *fp = fopen("/tmp/test_filedirmon.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", n++);
        fclose(fp);
    }
    fdmon.monitor("/tmp/test_filedirmon.txt", FEAL_FDM_CLOSE_WRITE);
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
    fdmon.close_and_reset();
    shutdown();
}

void ActorA::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtDelayTimer\n");
    FILE *fp = fopen("/tmp/test_filedirmon.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", n++);
        fclose(fp);
    }
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void ActorA::handleEvent(std::shared_ptr<EvtFDMReadAvail> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtFDMReadAvail\n");
    printf("filedirmon event for event %s\n", 
        (pevt->flags && FEAL_FDM_CLOSE_WRITE == FEAL_FDM_CLOSE_WRITE ? "FEAL_FDM_CLOSE_WRITE" : "something else"));
    printf("File contents of /tmp/test_filedirmon.txt: ");
    FILE *fp = fopen("/tmp/test_filedirmon.txt", "r");
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

