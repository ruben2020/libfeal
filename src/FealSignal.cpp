//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

feal::map_evtsig_t feal::SignalGeneric::map_signal;
std::mutex feal::SignalGeneric::mtx_map_sig;

void feal::SignalGeneric::init(void)
{
    const std::lock_guard<std::mutex> lock(mtx_map_sig);
    BaseSignal::recvsig_fp = &sighandler;
}

void feal::SignalGeneric::sighandler(int signum, int sicode)
{
    const std::lock_guard<std::mutex> lock(mtx_map_sig);
    auto it = map_signal.find(signum);
    if (it != map_signal.end())
    {
        vec_evtsig_ptr_t ves = it->second;
        for (auto itv = ves.begin(); itv != ves.end(); ++itv)
        {
            auto itw = std::dynamic_pointer_cast<EventSignal>(
                    EventBus::getInstance().cloneEvent(*itv));
            itw.get()->signo = signum;
            itw.get()->sicode = sicode;
            EventBus::getInstance().publishEvent(itw);
        }
    }
}
