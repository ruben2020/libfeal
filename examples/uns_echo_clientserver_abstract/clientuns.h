//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef CLIENTUNS_H
#define CLIENTUNS_H

#include "client.h"
#include "feal.h"

class Clientuns : public Client
{
   public:
    Clientuns() = default;
    ~Clientuns() override = default;

   protected:
    void connectToServer(void) override;
};

#endif  // _CLIENTUNS_H
