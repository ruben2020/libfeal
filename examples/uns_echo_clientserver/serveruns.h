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
 
#ifndef _SERVERUNS_H
#define _SERVERUNS_H

#include "feal.h"
#include "server.h"

class Serveruns : public Server
{

public:

Serveruns() = default;
~Serveruns() = default;


protected:

void start_server(void);
void print_client_address(feal::socket_t fd);
void get_client_address(feal::socket_t fd, char* addr);

};

#endif // _SERVERUNS_H
