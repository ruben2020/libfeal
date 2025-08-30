//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef CLIENT6_H
#define CLIENT6_H

#include "client.h"
#include "feal.h"

class Client6 : public Client
{
   public:
    Client6() = default;
    ~Client6() override = default;

   protected:
    void sendToServer(void) override;
};

#endif  // _CLIENT6_H
