// Please see the "examples/LICENSE" file for
// the copyright notice and license text.

#include <cstdio>
#include <cstring>
#include "server6.h"

void Server6::start_listening(void)
{
    feal::ipaddr serveraddr;
    feal::errenum se;
    serveraddr.family = feal::ipaddr::INET6;
    serveraddr.port = 12001;
    strcpy(serveraddr.addr, "::1");
    se = dgram.create_sock((feal::family_t) serveraddr.family);
    if (se != feal::FEAL_OK)
    {
        printf("create sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    se = dgram.bind_sock(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("bind sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("UDP Listening on %s:%d ...\n", serveraddr.addr, serveraddr.port );
}


