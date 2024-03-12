//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _FEAL_BASESIGNAL_H
#define _FEAL_BASESIGNAL_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <signal.h>

namespace feal
{

class BaseSignal : public Tool
{
public:
BaseSignal() = default;
BaseSignal( const BaseSignal & ) = default;
BaseSignal& operator= ( const BaseSignal & ) = default;
~BaseSignal() = default;

protected:
static void (*recvsig_fp) (int,int);

int do_registersignal(int signum);
int do_deregistersignal(int signum);

private:

#if defined (_WIN32)
static void win_sighandler(int sig);
#else
static void posix_sighandler(int sig, siginfo_t *info, void *ucontext);
#endif

};


} // namespace feal


#endif // _FEAL_BASESIGNAL_H
