//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include "fiforeader.h"

#define FIFOPATH "/tmp/fealfifo"

feal::EventId_t EvtEndTimer::getId(void)
{
    return getIdOfType<EvtEndTimer>();
}

feal::EventId_t EvtRetryTimer::getId(void)
{
    return getIdOfType<EvtRetryTimer>();
}

feal::EventId_t EvtFifoRead::getId(void)
{
    return getIdOfType<EvtFifoRead>();
}

void Fiforeader::initActor(void)
{
    printf("Fiforeader::initActor\n");
    timers.init(this);
    reader.init(this);
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
    res = reader.open_pipe_for_reading<EvtFifoRead>(FIFOPATH);
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
        pevt.get()->readerfd, pevt.get()->error, pevt.get()->datalen);
    if (pevt.get()->error)
    {
        open_for_reading();
        return;
    }
    printf("Data available for reading: %d\n", pevt.get()->datalen);
    char buf[100];
    read(pevt.get()->readerfd, buf, sizeof(buf));
    printf("Read from fifo: %s\n", buf);
}
