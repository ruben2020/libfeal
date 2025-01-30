//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _SERVER6_H
#define _SERVER6_H

#include "feal.h"
#include "server.h"

class Server6 : public Server
{

public:

Server6() = default;
~Server6() = default;


protected:

void start_listening(void);

};

#endif // _SERVER6_H
