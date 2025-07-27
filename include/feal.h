//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
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
#include "internal/FealDatagram.h"
#include "internal/FealBaseSignal.h"
#include "internal/FealSignal.h"
#ifndef _WIN32
#include "internal/FealReader.h"
#include "internal/FealFifoReader.h"
#include "internal/FealDescMon.h"
#include "internal/FealFileDirMon.h"
#endif

#endif // _FEAL_H
