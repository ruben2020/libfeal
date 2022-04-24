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
#include "clientuns.h"

int main(void)
{
    printf("unix domain socket stream client starting program\n");
    std::shared_ptr<feal::Actor> unsclient1 = std::make_shared<Clientuns>();
    unsclient1.get()->init();
    unsclient1.get()->start();
    unsclient1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("unix domain socket stream client terminating program\n");
    return 0;
}

