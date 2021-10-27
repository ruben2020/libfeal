/*
 * Copyright (c) 2020 ruben2020 https://github.com/ruben2020
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#if defined (_WIN32)
#include "FealCommon_win.cpp"
#elif defined (__linux__)
#include "FealCommon_posix.cpp"
#elif defined (__APPLE__)   || defined (__FreeBSD__)   || defined (__NetBSD__) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealCommon_posix.cpp"
#else
#error "Unsupported operating system"
#endif

