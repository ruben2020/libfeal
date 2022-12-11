//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR LGPL-2.1-or-later
//
 
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

