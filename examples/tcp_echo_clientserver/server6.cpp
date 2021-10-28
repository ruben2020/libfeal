// Please see the "examples/LICENSE" file for
// the copyright notice and license text.

#include <cstdio>
#include <cstring>
#include "clienthandler.h"
#include "server6.h"

void Server6::start_server(void)
{
    feal::ipaddr serveraddr;
    serveraddr.family = feal::ipaddr::INET6;
    serveraddr.port = 11001;
    strcpy(serveraddr.addr, "::1");
    printf("Starting Server on ::1 port 11001\n");
    feal::errenum se = stream.create_and_bind(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("Error binding to ::1 port 11001  err %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    se = stream.listen();
    if (se != feal::FEAL_OK)
    {
        printf("Error listening to ::1 port 11001  err %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Listening ...\n");
}


