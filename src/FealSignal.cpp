/*
 * Copyright (c) 2020 ruben2020 https://github.com/ruben2020
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "feal.h"

feal::map_evtsig_t feal::SignalGeneric::mapSignal;
std::mutex feal::SignalGeneric::mtxMapSig;

void feal::SignalGeneric::init(void)
{
    const std::lock_guard<std::mutex> lock(mtxMapSig);
    BaseSignal::recvsig_fp = &sighandler;
}

void feal::SignalGeneric::sighandler(int signum, int sicode)
{
    const std::lock_guard<std::mutex> lock(mtxMapSig);
    auto it = mapSignal.find(signum);
    if (it != mapSignal.end())
    {
        vec_evtsig_ptr_t ves = it->second;
        for (auto itv = ves.begin(); itv != ves.end(); ++itv)
        {
            auto itw = std::dynamic_pointer_cast<EventSignal>(EventBus::getInstance().cloneEvent(*itv));
            itw.get()->signo = signum;
            itw.get()->sicode = sicode;
            EventBus::getInstance().publishEvent(itw);
        }
    }
}
