//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
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
void get_client_address(feal::handle_t fd, char* addr);

};

#endif // _SERVERUNS_H
