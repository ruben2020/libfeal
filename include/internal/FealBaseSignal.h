/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Dual-licensed under the Apache License, Version 2.0, and
 * the GNU General Public License, Version 2.0;
 * you may not use this file except in compliance
 * with either one of these licenses.
 *
 * You can find copies of these licenses in the included
 * LICENSE-APACHE2 and LICENSE-GPL2 files.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under these licenses is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the license texts for the specific language governing permissions
 * and limitations under the licenses.
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
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
