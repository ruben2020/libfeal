#include <cstdio>
#include <cstring>
#include "clienthandler.h"
#include "serveruns.h"

#define UNS(x) (x == AF_UNIX ? "AF_UNIX" : "unknown")
#define SERVERPATH "/tmp/fealunsserver"

void Serveruns::start_server(void)
{
    struct sockaddr_un serveraddr;
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SERVERPATH);
    unlink(SERVERPATH);
    printf("Starting Server on %s\n", SERVERPATH);
    feal::sockerrenum se = stream.create_and_bind(&serveraddr);
    if (se != feal::S_OK)
    {
        printf("Error binding to %s  err %d\n", SERVERPATH, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    se = stream.listen_sock();
    if (se != feal::S_OK)
    {
        printf("Error listening to %s  err %d\n", SERVERPATH, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Listening ...\n");
}

void Serveruns::print_client_address(feal::socket_t fd)
{
#if defined (__linux__)
    socklen_t len;
    struct ucred ucred;

    len = sizeof(struct ucred);

    if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len) == -1) {
        //getsockopt failed
        return;
    }

    printf("Credentials from SO_PEERCRED: pid=%ld, euid=%ld, egid=%ld\n",
        (long) ucred.pid, (long) ucred.uid, (long) ucred.gid);
#else
    uid_t euid=0;
    gid_t egid=0;
    getpeereid(fd, &euid, &egid);
    printf("Credentials from getpeerid: euid=%ld, egid=%ld\n",
        (long) euid, (long) egid);
#endif
}
