//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

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
    void print_client_address(feal::handle_t fd);
    void get_client_address(feal::handle_t fd, char* addr, int addrbuflen);
};

#endif  // _SERVERUNS_H
