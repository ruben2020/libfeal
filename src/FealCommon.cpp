/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Licensed under the Apache License, Version 2.0
 * with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You can find a copy of the License in the LICENSE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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

