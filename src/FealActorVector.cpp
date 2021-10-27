/*
 * Copyright (c) 2020 ruben2020 https://github.com/ruben2020
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "feal.h"

void feal::initAll(feal::actor_vec_t& vec)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).init();
    }
}

void feal::startAll(feal::actor_vec_t& vec)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).start();
    }
}

void feal::pauseAll(feal::actor_vec_t& vec)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).pause();
    }
}

void feal::shutdownAll(feal::actor_vec_t& vec)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).shutdown();
    }
}

void feal::receiveEventAll(feal::actor_vec_t& vec, std::shared_ptr<feal::Event> pevt)
{
    if (!pevt) return;
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        (**it).receiveEvent(pevt);
    }
}
