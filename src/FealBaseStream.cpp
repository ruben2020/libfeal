/*
 * Copyright (c) 2020 ruben2020 https://github.com/ruben2020
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#if defined (_WIN32)
#include "FealBaseStream_win.cpp"
#elif defined (__linux__)
#include "FealBaseStream_epoll.cpp"
#elif defined (__APPLE__)   || defined (__FreeBSD__) || defined (__NetBSD_) || \
      defined (__OpenBSD__) || defined (__DragonFly__)
#include "FealBaseStream_kqueue.cpp"
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include "FealBaseStream_kqueue.cpp"
#else
#error "Unsupported operating system"
#endif
