//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <sys/inotify.h>
#include <linux/limits.h>
#include "ActorA.h"


void ActorA::initActor(void)
{
    printf("ActorA::initActor\n");
    timers.init(this);
    dmon.init(this);
    dmon.subscribeReadAvail<EvtINotifyReadAvail>();
    dmon.subscribeWriteAvail<EvtINotifyWriteAvail>();
    dmon.subscribeDescErr<EvtINotifyErr>();
}

void ActorA::startActor(void)
{
    printf("ActorA::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(20));
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
    FILE *fp = fopen("/tmp/test_inotifymon.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", n++);
        fclose(fp);
    }
    fd = inotify_init();
    if (fd == -1)
    {
        printf("inotify_init error\n");
        return;
    }
    wd = inotify_add_watch(fd, "/tmp/test_inotifymon.txt", IN_CLOSE_WRITE);
    printf("Watch descriptor for IN_CLOSE_WRITE for /tmp/test_inotifymon.txt = %d\n", wd);
    dmon.monitor(fd);
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
    inotify_rm_watch(fd, wd);
    dmon.close_and_reset();
    shutdown();
}

void ActorA::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtDelayTimer\n");
    FILE *fp = fopen("/tmp/test_inotifymon.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", n++);
        fclose(fp);
    }
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void ActorA::handleEvent(std::shared_ptr<EvtINotifyReadAvail> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtINotifyReadAvail\n");
    size_t bufsize = sizeof(struct inotify_event) + NAME_MAX + 1;
    struct inotify_event* event = new struct inotify_event[bufsize / sizeof(struct inotify_event)];
    if (read(fd, event, bufsize) > 0)
    {
        printf("inotify read event with watch descriptor = %d for event %s\n", 
            event[0].wd, (event[0].mask && IN_CLOSE_WRITE == IN_CLOSE_WRITE ? "IN_CLOSE_WRITE" : "something else"));
        printf("File contents of /tmp/test_inotifymon.txt: ");
        FILE *fp = fopen("/tmp/test_inotifymon.txt", "r");
        if (fp != NULL)
        {
            int val=0;
            fscanf(fp, "%d\n", &val);
            printf("%d\n", val);
            fclose(fp);
        }
    }
    delete[] event;
}

void ActorA::handleEvent(std::shared_ptr<EvtINotifyWriteAvail> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtINotifyWriteAvail\n");
}

void ActorA::handleEvent(std::shared_ptr<EvtINotifyErr> pevt)
{
    if (!pevt) return;
    printf("ActorA::EvtINotifyErr\n");
    timers.stopTimer<EvtDelayTimer>();
    dmon.close_and_reset();
}

