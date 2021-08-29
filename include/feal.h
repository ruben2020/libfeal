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
    typedef uint32_t ActorId_t;
}

#include "internal/FealTool.h"
#include "internal/FealEvent.h"
#include "internal/FealEventBus.h"
#include "internal/FealTimer.h"
#include "internal/FealTimers.h"
#include "internal/FealActor.h"
#include "internal/FealActorVector.h"
//#include "internal/FealStream.h"
//#include "internal/FealTCPStream.h"


#endif // _FEAL_H
