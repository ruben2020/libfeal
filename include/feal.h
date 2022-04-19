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

#endif // _FEAL_H
