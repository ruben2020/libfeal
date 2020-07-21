#ifndef _FEAL_H
#define _FEAL_H

#include <stdio.h>
#include <cstdint>

#define FEALDEBUG (1)

#if (FEALDEBUG)
#define FEALDEBUGLOG(x)      printf("%s\n", x);
#else
#define FEALDEBUGLOG(x)
#endif

namespace feal
{
    typedef uint32_t EventId_t;
}

#include "internal/FealEvent.h"
#include "internal/FealEventBus.h"
#include "internal/FealActor.h"


#endif // _FEAL_H
