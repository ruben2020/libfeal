/*
 * Copyright (c) 2020 ruben2020 https://github.com/ruben2020
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#ifndef _FEAL_ACTOR_VECTOR_H
#define _FEAL_ACTOR_VECTOR_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <memory>
#include <vector>

namespace feal
{

class Actor;
class Event;

typedef  std::shared_ptr<Actor> actorptr_t;
typedef std::vector<actorptr_t> actor_vec_t;

void initAll(actor_vec_t& vec);
void startAll(actor_vec_t& vec);
void pauseAll(actor_vec_t &vec);
void shutdownAll(actor_vec_t& vec);
void receiveEventAll(actor_vec_t& vec, std::shared_ptr<Event> pevt);

} // namespace feal


#endif // _FEAL_ACTOR_VECTOR_H
