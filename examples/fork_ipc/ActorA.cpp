//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <cstdio>
#include <sys/wait.h>
#include "feal.h"
#include "ActorA.h"
#include "ActorsManager.h"

feal::EventId_t EvtSigInt::getId(void)
{
    return getIdOfType<EvtSigInt>();
}

feal::EventId_t EvtSigChld::getId(void)
{
    return getIdOfType<EvtSigChld>();
}

feal::EventId_t EvtPipeRead::getId(void)
{
    return getIdOfType<EvtPipeRead>();
}

feal::EventId_t EvtSockStreamRead::getId(void)
{
    return getIdOfType<EvtSockStreamRead>();
}

feal::EventId_t EvtSockDatagramRead::getId(void)
{
    return getIdOfType<EvtSockDatagramRead>();
}


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
    forkPipeChild();
    forkSockStreamChild();
    forkSockDatagramChild();
}

void ActorA::forkPipeChild(void)
{
    pid_t p;
    feal::handle_t fd[2];
    if (pipe2(fd, O_NONBLOCK) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return;
    }
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
        printf("Child 1: Starting child process\n");
        for(int i=0; i<20; i++)
        {
            sprintf(buf, "Child 1, hello %d", i);
            printf("Child 1: Sending \"%s\" to parent using pipe\n", buf);
            write(fd[1], buf, strlen(buf) + 1);
            std::this_thread::sleep_for(std::chrono::seconds(2));            
        }
        printf("Child 1: Terminating child process\n");
        close(fd[1]);
        exit(0);
    }
    else // Parent process
    {
        close(fd[1]);
        readerPipe.subscribeReader<EvtPipeRead>(fd[0]);
        printf("Child process pid %d forked\n", p);
        pidvec.push_back(p);
    }
}

void ActorA::forkSockStreamChild(void)
{
    pid_t p;
    feal::socket_t fd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, fd) == -1)
    {
        fprintf(stderr, "Socket Failed");
        return;
    }
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
        printf("Child 2: Starting child process\n");
        for(int i=0; i<20; i++)
        {
            sprintf(buf, "Child 2, hello %d", i);
            printf("Child 2: Sending \"%s\" to parent using sock stream\n", buf);
            write(fd[1], buf, strlen(buf) + 1);
            std::this_thread::sleep_for(std::chrono::seconds(2));            
        }
        printf("Child 2: Terminating child process\n");
        close(fd[1]);
        exit(0);
    }
    else // Parent process
    {
        close(fd[1]);
        readerSockStream.subscribeReader<EvtSockStreamRead>(fd[0]);
        printf("Child process pid %d forked\n", p);
        pidvec.push_back(p);
    }
}

void ActorA::forkSockDatagramChild(void)
{
    pid_t p;
    feal::socket_t fd[2];
    if (socketpair(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0, fd) == -1)
    {
        fprintf(stderr, "Socket Failed");
        return;
    }
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
        printf("Child 3: Starting child process\n");
        for(int i=0; i<20; i++)
        {
            sprintf(buf, "Child 3, hello %d", i);
            printf("Child 3: Sending \"%s\" to parent using sock datagram\n", buf);
            write(fd[1], buf, strlen(buf) + 1);
            std::this_thread::sleep_for(std::chrono::seconds(2));            
        }
        printf("Child 3: Terminating child process\n");
        close(fd[1]);
        exit(0);
    }
    else // Parent process
    {
        close(fd[1]);
        readerSockDatagram.subscribeReader<EvtSockDatagramRead>(fd[0]);
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
        pevt.get()->readerfd, pevt.get()->error, pevt.get()->datalen);
    if (pevt.get()->error)
    {
        readerPipe.close_and_reset();
        printf("Pipe error. Closed\n");
        return;
    }
    printf("EvtPipeRead: Data available for reading: %d\n", pevt.get()->datalen);
    char buf[100];
    read(pevt.get()->readerfd, buf, sizeof(buf));
    printf("EvtPipeRead: Read from pipe: %s\n", buf);
}

void ActorA::handleEvent(std::shared_ptr<EvtSockStreamRead> pevt)
{
    if (!pevt ) return;
    printf("ActorA::EvtSockStreamRead\n");
    printf("hEvtSockStreamRead: andle %d, error %d, data avail %d\n", 
        pevt.get()->readerfd, pevt.get()->error, pevt.get()->datalen);
    if (pevt.get()->error)
    {
        readerSockStream.close_and_reset();
        printf("Socket error. Closed\n");
        return;
    }
    printf("EvtSockStreamRead: Data available for reading: %d\n", pevt.get()->datalen);
    char buf[100];
    read(pevt.get()->readerfd, buf, sizeof(buf));
    printf("EvtSockStreamRead: Read from sock stream: %s\n", buf);
}

void ActorA::handleEvent(std::shared_ptr<EvtSockDatagramRead> pevt)
{
    if (!pevt ) return;
    printf("ActorA::EvtSockDatagramRead\n");
    printf("EvtSockDatagramRead: handle %d, error %d, data avail %d\n", 
        pevt.get()->readerfd, pevt.get()->error, pevt.get()->datalen);
    if (pevt.get()->error)
    {
        readerSockDatagram.close_and_reset();
        printf("Socket error. Closed\n");
        return;
    }
    printf("EvtSockDatagramRead: Data available for reading: %d\n", pevt.get()->datalen);
    char buf[100];
    read(pevt.get()->readerfd, buf, sizeof(buf));
    printf("EvtSockDatagramRead: Read from sock datagram: %s\n", buf);
}

