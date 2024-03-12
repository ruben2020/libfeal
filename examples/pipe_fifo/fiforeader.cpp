//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include "fiforeader.h"

#define FIFOPATH "/tmp/fealfifo"


void Fiforeader::initActor(void)
{
    printf("Fiforeader::initActor\n");
    timers.init(this);
    reader.init(this);
    reader.subscribeReadAvail<EvtFifoRead>();
}

void Fiforeader::startActor(void)
{
    printf("Fiforeader::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(80));
    open_for_reading();
}

void Fiforeader::pauseActor(void)
{
    printf("Fiforeader::pauseActor\n");
}

void Fiforeader::shutdownActor(void)
{
    printf("Fiforeader::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("Client shutdown complete\n");
}

void Fiforeader::open_for_reading(void)
{
    feal::errenum res = feal::FEAL_OK;
    res = reader.open(FIFOPATH);
    if (res != feal::FEAL_OK)
    {
        printf("Error opening file for reading with errno %d\n", errno);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(2));
    }
}

void Fiforeader::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("Fiforeader::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtRetryTimer>();
    reader.close_and_reset();
    shutdown();
}

void Fiforeader::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt) return;
    printf("Fiforeader::EvtRetryTimer\n");
    open_for_reading();
}

void Fiforeader::handleEvent(std::shared_ptr<EvtFifoRead> pevt)
{
    if (!pevt) return;
    printf("Fiforeader::EvtFifoRead\n");
    printf("handle %d, error %d, data avail %d\n", 
        pevt.get()->fd, pevt.get()->errnum, pevt.get()->datalen);
    if (pevt.get()->errnum != feal::FEAL_OK)
    {
        open_for_reading();
        return;
    }
    printf("Data available for reading: %d\n", pevt.get()->datalen);
    char buf[100];
    read(pevt.get()->fd, buf, sizeof(buf));
    printf("Read from fifo: %s\n", buf);
}
