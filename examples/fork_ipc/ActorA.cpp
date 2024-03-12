//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <sys/wait.h>

#if defined(__APPLE__) || defined(__MACH__)
#include <fcntl.h>
#define SOCK_NONBLOCK O_NONBLOCK
#endif

#include "feal.h"
#include "ActorA.h"
#include "ActorsManager.h"


void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
    signal.init(this);
    readerPipe.init(this);
    readerSockStream.init(this);
    readerSockDatagram.init(this);
    signal.registerSignal<EvtSigInt>(SIGINT);
    signal.registerSignal<EvtSigInt>(SIGTERM);
    signal.registerSignal<EvtSigChld>(SIGCHLD);
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");
    forkChild(1, "pipe");
    forkChild(2, "sock stream");
    forkChild(3, "sock datagram");
}

void ActorA::forkChild(int childnum, const char* medium)
{
    pid_t p;
    feal::handle_t fd[2]; // handle_t and handle_t are the same
    int ret = -1;
    switch(childnum)
    {
        case 1:
#if defined(__APPLE__) || defined(__MACH__)
            ret = pipe(fd);
            fcntl(fd[0], F_SETFL,
                fcntl(fd[0], F_GETFL) |
                O_NONBLOCK);
            fcntl(fd[1], F_SETFL,
                fcntl(fd[1], F_GETFL) |
                O_NONBLOCK);
#else
            ret = pipe2(fd, O_NONBLOCK);
#endif
            break;
        case 2:
            ret = socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, fd);
            break;
        case 3:
            ret = socketpair(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0, fd);
            break;
        default:
            break;
    }
    if (ret == -1) printf("Error creating socket/ pipe\n");
    p = fork();
    if (p < 0)
    {
        printf("Fork error occurred!\n");
        return;
    }
    if (p == 0) // Child process
    {
        char buf[30];
        close(fd[0]);
        printf("Child %d: Starting child process\n", childnum);
        for(int i=0; i<20; i++)
        {
            snprintf(buf, sizeof(buf), "Child %d, hello %d", childnum, i);
            printf("Child %d: Sending \"%s\" to parent using %s\n", childnum, buf, medium);
            write(fd[1], buf, strlen(buf) + 1);
            std::this_thread::sleep_for(std::chrono::seconds(2));            
        }
        printf("Child %d: Terminating child process\n", childnum);
        close(fd[1]);
        exit(0);
    }
    else // Parent process
    {
    close(fd[1]);
    switch(childnum)
    {
        case 1:
            readerPipe.subscribeReadAvail<EvtPipeRead>(fd[0]);
            break;
        case 2:
            readerSockStream.subscribeReadAvail<EvtSockStreamRead>(fd[0]);
            break;
        case 3:
            readerSockDatagram.subscribeReadAvail<EvtSockDatagramRead>(fd[0]);
            break;
        default:
            break;
    }
        printf("Child process pid %d forked\n", p);
        pidvec.push_back(p);
    }
}

void ActorA::pauseActor(void)
{
    printf("ActorA pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA shutdownActor\n");
}

void ActorA::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt ) return;
    printf("ActorA::EvtSigInt (signum=%d, sicode=%d)\n", 
        pevt.get()->signo, pevt.get()->sicode);
    switch(pevt.get()->signo)
    {
        case SIGINT:
            printf("Received SIGINT\n");
            break;

        case SIGTERM:
            printf("Received SIGTERM\n");
            break;

        default:
            break;
    }
    for(long unsigned int i=0; i<pidvec.size(); i++)
    {
        if (pidvec[i] > 0) 
        {
            printf("Killing child process pid %d\n", pidvec[i]);
            kill(pidvec[i], SIGKILL);
        }
    }
    std::shared_ptr<feal::Event> pevt2 = std::make_shared<EvtAllDone>();
    publishEvent(pevt2);
}

void ActorA::handleEvent(std::shared_ptr<EvtSigChld> pevt)
{
    if (!pevt ) return;
    printf("ActorA::EvtSigChld (signum=%d, sicode=%d)\n", 
        pevt.get()->signo, pevt.get()->sicode);
    if (pevt.get()->signo != SIGCHLD) return;
    pid_t p;
    int wstatus = 0;

    /* Reap all pending child processes */
    do {
        p = waitpid(-1, &wstatus, WNOHANG);
        if (p <= (pid_t)0) continue;
        printf("Child process pid %d terminated %s", p,
            (WIFEXITED(wstatus)?"normally":"abnormally\n"));
        if (WIFEXITED(wstatus))
        {
            printf(" with exit code %d\n", WEXITSTATUS(wstatus));
        }
        for(long unsigned int i=0; i<pidvec.size(); i++)
        {
            if (pidvec[i] == p) 
            {
                pidvec[i] = 0;
                break;
            }
        }
    } while ((p != (pid_t)0) && (p != (pid_t)-1));
    if (p == (pid_t)-1)
    {
        std::shared_ptr<feal::Event> pevt2 = std::make_shared<EvtAllDone>();
        publishEvent(pevt2);
    }
}

void ActorA::handleEvent(std::shared_ptr<EvtPipeRead> pevt)
{
    if (!pevt ) return;
    printf("ActorA::EvtPipeRead\n");
    printf("EvtPipeRead: handle %d, error %d, data avail %d\n", 
        pevt.get()->fd, pevt.get()->errnum, pevt.get()->datalen);
    if (pevt.get()->errnum != feal::FEAL_OK)
    {
        readerPipe.close_and_reset();
        printf("Pipe error. Closed\n");
        return;
    }
    printf("EvtPipeRead: Data available for reading: %d\n", pevt.get()->datalen);
    char buf[100];
    read(pevt.get()->fd, buf, sizeof(buf));
    printf("EvtPipeRead: Read from pipe: %s\n", buf);
}

void ActorA::handleEvent(std::shared_ptr<EvtSockStreamRead> pevt)
{
    if (!pevt ) return;
    printf("ActorA::EvtSockStreamRead\n");
    printf("hEvtSockStreamRead: andle %d, error %d, data avail %d\n", 
        pevt.get()->fd, pevt.get()->errnum, pevt.get()->datalen);
    if (pevt.get()->errnum != feal::FEAL_OK)
    {
        readerSockStream.close_and_reset();
        printf("Socket error. Closed\n");
        return;
    }
    printf("EvtSockStreamRead: Data available for reading: %d\n", pevt.get()->datalen);
    char buf[100];
    read(pevt.get()->fd, buf, sizeof(buf));
    printf("EvtSockStreamRead: Read from sock stream: %s\n", buf);
}

void ActorA::handleEvent(std::shared_ptr<EvtSockDatagramRead> pevt)
{
    if (!pevt ) return;
    printf("ActorA::EvtSockDatagramRead\n");
    printf("EvtSockDatagramRead: handle %d, error %d, data avail %d\n", 
        pevt.get()->fd, pevt.get()->errnum, pevt.get()->datalen);
    if (pevt.get()->errnum != feal::FEAL_OK)
    {
        readerSockDatagram.close_and_reset();
        printf("Socket error. Closed\n");
        return;
    }
    printf("EvtSockDatagramRead: Data available for reading: %d\n", pevt.get()->datalen);
    char buf[100];
    read(pevt.get()->fd, buf, sizeof(buf));
    printf("EvtSockDatagramRead: Read from sock datagram: %s\n", buf);
}

