// Please see the "examples/LICENSE" file for
// the copyright notice and license text.

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
