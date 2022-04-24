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
