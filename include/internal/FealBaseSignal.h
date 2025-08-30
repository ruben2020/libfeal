//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_BASESIGNAL_H
#define FEAL_BASESIGNAL_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <signal.h>

namespace feal
{

class BaseSignal : public Tool
{
   public:
    BaseSignal() = default;
    BaseSignal(const BaseSignal&) = default;
    BaseSignal& operator=(const BaseSignal&) = default;
    ~BaseSignal() override = default;

   protected:
    static void (*recvsig_fp)(int, int);

    int doRegisterSignal(int signum);
    int doDeregisterSignal(int signum);

   private:
#if defined(_WIN32)
    static void winSigHandler(int sig);
#else
    static void posixSigHandler(int sig, siginfo_t *info, void *ucontext);
#endif
};

}  // namespace feal

#endif  // FEAL_BASESIGNAL_H
