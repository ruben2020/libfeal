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

