//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include <cstdio>
#include <sys/wait.h>
#include "feal.h"
#include "ActorA.h"
#include "ActorsManager.h"


void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
    signal.init(this);
    signal.registerSignal<EvtSigInt>(SIGINT);
    signal.registerSignal<EvtSigInt>(SIGTERM);
    signal.registerSignal<EvtSigChld>(SIGCHLD);
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");
    pid_t p;
    for (int i = 0; i < 5; i++)
    {
        p = fork();
        if (p < 0)
        {
            printf("Fork error occurred!\n");
            return;
        }
        if (p == 0) // Child process
        {
            printf("Child: Starting child process %d\n", i + 1);
            std::this_thread::sleep_for(std::chrono::milliseconds((i+1)*2000));
            printf("Child: Terminating child process %d\n", i + 1);
            exit(i);
        }
        else
        {
            printf("Child process pid %d forked\n", p);
            pidvec.push_back(p);
        }
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
