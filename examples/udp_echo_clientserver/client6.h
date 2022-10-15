//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#ifndef _CLIENT6_H
#define _CLIENT6_H

#include "feal.h"
#include "client.h"

class Client6 : public Client
{

public:

Client6() = default;
~Client6() = default;


protected:

void send_to_server(void);

};

#endif // _CLIENT6_H

