//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR LGPL-2.1-or-later
//
 
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

