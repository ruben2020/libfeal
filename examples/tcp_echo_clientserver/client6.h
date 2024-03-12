//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
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

void connect_to_server(void);

};

#endif // _CLIENT6_H

