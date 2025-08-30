//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

void (*feal::BaseSignal::recvsig_fp)(int, int) = nullptr;

#if defined(_WIN32)

int feal::BaseSignal::doRegisterSignal(int signum)
{
    signal(signum, &winSigHandler);
    return 0;
}

int feal::BaseSignal::doDeregisterSignal(int signum)
{
    signal(signum, nullptr);
    return 0;
}

void feal::BaseSignal::winSigHandler(int sig)
{
    if (recvsig_fp)
        recvsig_fp(sig, 0);
}

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || \
        defined(__DragonFly__) || defined(__linux__)

int feal::BaseSignal::doRegisterSignal(int signum)
{
    struct sigaction sa;
    sa.sa_sigaction = &posixSigHandler;
    sigemptyset(&(sa.sa_mask));
    sa.sa_flags = (SA_RESTART | SA_SIGINFO | (signum == SIGCHLD ? SA_NOCLDSTOP : 0));
    return sigaction(signum, &sa, nullptr);
}

int feal::BaseSignal::doDeregisterSignal(int signum)
{
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;  // go back to default
    sigemptyset(&(sa.sa_mask));
    sa.sa_flags = 0;
    return sigaction(signum, &sa, nullptr);
}

void feal::BaseSignal::posixSigHandler(int sig, siginfo_t *info, void *ucontext)
{
    (void)ucontext;
    int sicode = -1;
    if (info)
        sicode = info->si_code;
    if (recvsig_fp)
        recvsig_fp(sig, sicode);
}

#else
#error "Unsupported operating system"
#endif
