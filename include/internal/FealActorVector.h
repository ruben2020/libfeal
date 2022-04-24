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
