#ifndef _FEAL_BASESIGNAL_POSIX_H
#define _FEAL_BASESIGNAL_POSIX_H

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

static void posix_sighandler(int sig, siginfo_t *info, void *ucontext);

};


} // namespace feal


#endif // _FEAL_BASESIGNAL_POSIX_H
