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
 
#include <stdio.h>
#include "clientund.h"

int main(void)
{
    printf("Unix domain socket datagram client starting program\n");
    std::shared_ptr<feal::Actor> undclient1 = std::make_shared<Clientund>();
    undclient1.get()->init();
    undclient1.get()->start();
    undclient1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("Unix domain socket datagram client terminating program\n");
    return 0;
}

