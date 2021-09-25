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

#include "internal/FealCommon.h"
#include "internal/FealTool.h"
#include "internal/FealEvent.h"
#include "internal/FealEventBus.h"
#include "internal/FealTimer.h"
#include "internal/FealTimers.h"
#include "internal/FealActor.h"
#include "internal/FealActorVector.h"
#include "internal/FealBaseStream.h"
#include "internal/FealStream.h"
//#include "internal/FealDatagram.h"
//#include "internal/FealBaseSignal.h"
//#include "internal/FealSignal.h"

#endif // _FEAL_H
