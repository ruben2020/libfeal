/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Licensed under the Apache License, Version 2.0
 * with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You can find a copy of the License in the LICENSE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 * 
 */
 
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
