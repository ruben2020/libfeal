#ifndef _CLIENTUNS_H
#define _CLIENTUNS_H

#include "feal.h"
#include "client.h"

class Clientuns : public Client
{

public:

Clientuns() = default;
~Clientuns() = default;


protected:

void connect_to_server(void);

};

#endif // _CLIENTUNS_H

