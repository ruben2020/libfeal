#include <cstdio>
#include <cstring>
#include "client6.h"

void Client6::connect_to_server(void)
{
    feal::ipaddr serveraddr;
    serveraddr.family = feal::ipaddr::INET6;
    serveraddr.port = 11001;
    strcpy(serveraddr.addr, "::1");
    printf("Trying to connect ::1 port 11001\n");
    feal::errenum se = stream.create_and_connect(&serveraddr);
    if (se != feal::S_OK)
    {
        printf("Error connecting to ::1 port 11001  err %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}


