//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "fifowriter.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <cstdio>
#include <cstring>

#define FIFOPATH "/tmp/fealfifo"

void Fifowriter::initActor(void)
{
    printf("Fifowriter::initActor\n");
    timers.init(this);
}

void Fifowriter::startActor(void)
{
    printf("Fifowriter::startActor\n");
    signal(SIGPIPE, SIG_IGN);
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(12));
    if (access(FIFOPATH, F_OK | R_OK | W_OK) != 0)
        mkfifo(FIFOPATH, 0666);
    open_for_writing();
    send_something();
}

void Fifowriter::pauseActor(void)
{
    printf("Fifowriter::pauseActor\n");
}

void Fifowriter::shutdownActor(void)
{
    printf("Fifowriter::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("Fifowriter shutdown complete\n");
}

void Fifowriter::open_for_writing(void)
{
    printf("Opening pipe for writing\n");
    fifofd = open(FIFOPATH, O_RDWR);
    if (fifofd == -1)
    {
        printf("Error opening file for writing with errno %d\n", errno);
        timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
        return;
    }
    feal::set_nonblocking(fifofd);
}

void Fifowriter::send_something(void)
{
    char buf[30];
    ssize_t bytes;
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Fifowriter %d", n++);
    printf("Trying to send \"%s\" to %s\n", buf, FIFOPATH);
    bytes = write(fifofd, buf, strlen(buf) + 2);
    if (bytes > 0)
        printf("Wrote %lld bytes\n", (long long)bytes);
    else
    {
        printf("Error writing with errno %d\n", errno);
        open_for_writing();
    }
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void Fifowriter::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt)
        return;
    printf("Fifowriter::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtDelayTimer>();
    close(fifofd);
    shutdown();
}

void Fifowriter::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt)
        return;
    printf("Fifowriter::EvtDelayTimer\n");
    send_something();
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}
