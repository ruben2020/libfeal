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
 
#include <cstdio>
#include <cstring>
#include "server6.h"

void Server6::start_listening(void)
{
    feal::ipaddr serveraddr;
    feal::errenum se;
    serveraddr.family = feal::ipaddr::INET6;
    serveraddr.port = 12001;
    strcpy(serveraddr.addr, "::1");
    se = dgram.create_sock((feal::family_t) serveraddr.family);
    if (se != feal::FEAL_OK)
    {
        printf("create sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    se = dgram.bind_sock(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("bind sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("UDP Listening on %s:%d ...\n", serveraddr.addr, serveraddr.port );
}


