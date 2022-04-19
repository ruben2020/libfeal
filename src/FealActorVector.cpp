/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Dual-licensed under the Apache License, Version 2.0, and
 * the GNU General Public License, Version 2.0;
 * you may not use this file except in compliance
 * with either one of these licenses.
 *
 * You can find copies of these licenses in the included
 * LICENSE-APACHE2 and LICENSE-GPL2 files.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under these licenses is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the license texts for the specific language governing permissions
 * and limitations under the licenses.
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
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
