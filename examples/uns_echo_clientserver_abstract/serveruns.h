//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef SERVERUNS_H
#define SERVERUNS_H

#include "feal.h"
#include "server.h"

class Serveruns : public Server
{
   public:
    Serveruns() = default;
    ~Serveruns() override = default;

   protected:
    void startServer(void) override;
    void printClientAddress(feal::handle_t fd) override;
    void getClientAddress(feal::handle_t fd, char* addr, int addrbuflen) override;
};

#endif  // _SERVERUNS_H
