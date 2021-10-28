// Please see the "examples/LICENSE" file for
// the copyright notice and license text.

#include <cstdio>
#include <cstring>
#include "client.h"

#define MIN(a,b) (a<b ? a : b)

feal::EventId_t EvtEndTimer::getId(void)
{
    return getIdOfType<EvtEndTimer>();
}

feal::EventId_t EvtDelayTimer::getId(void)
{
    return getIdOfType<EvtDelayTimer>();
}

feal::EventId_t EvtRetryTimer::getId(void)
{
    return getIdOfType<EvtRetryTimer>();
}

feal::EventId_t EvtSigInt::getId(void)
{
    return getIdOfType<EvtSigInt>();
}

void Client::initActor(void)
{
    printf("Client::initActor\n");
    timers.init(this);
    stream.init(this);
    signal.init(this);
    signal.registersignal<EvtSigInt>(SIGINT);
}

void Client::startActor(void)
{
    printf("Client::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(45));
    connect_to_server();
}

void Client::pauseActor(void)
{
    printf("Client::pauseActor\n");
}

void Client::shutdownActor(void)
{
    printf("Client::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("Client shutdown complete\n");
}

void Client::connect_to_server(void)
{
    feal::ipaddr serveraddr;
    serveraddr.family = feal::ipaddr::INET;
    serveraddr.port = 11001;
    strcpy(serveraddr.addr, "127.0.0.1");
    printf("Trying to connect to 127.0.0.1:11001\n");
    feal::errenum se = stream.create_and_connect(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("Error connecting to 127.0.0.1:11001  err %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}

void Client::send_something(void)
{
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    sprintf(buf, "Client %d", n++);
    printf("Trying to send \"%s\"\n",buf);
    feal::errenum se = stream.send((void*) buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes);
    if (se != feal::FEAL_OK) printf("Error sending \"Client n\": %d\n", se);
}

void Client::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtEndTimer Elapsed\n");
    stream.disconnect_and_reset();
    shutdown();
}

void Client::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDelayTimer\n");
    send_something();
}

void Client::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtRetryTimer\n");
    connect_to_server();
}

void Client::handleEvent(std::shared_ptr<feal::EvtConnectedToServer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtConnectedToServer\n");
    char buf[30];
    strcpy(buf, "Hello! Client here!");
    int32_t bytes;
    printf("Trying to send \"Hello! Client here!\"\n");
    feal::errenum se = stream.send((void*) buf, sizeof(buf), &bytes);
    if (se != feal::FEAL_OK) printf("Error sending \"Hello! Client here!\": %d\n", se);
}

void Client::handleEvent(std::shared_ptr<feal::EvtDataReadAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDataReadAvail\n");
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::errenum se = stream.recv((void*) buf, sizeof(buf), &bytes);
    if (se != feal::FEAL_OK) printf("Error receiving: %d\n", se);
    else printf("Received %d bytes: \"%s\"\n", bytes, buf);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void Client::handleEvent(std::shared_ptr<feal::EvtDataWriteAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDataWriteAvail\n");
    send_something();
}

void Client::handleEvent(std::shared_ptr<feal::EvtConnectionShutdown> pevt)
{
    if (!pevt) return;
    printf("Client::EvtConnectionShutdown\n");
    timers.stopTimer<EvtDelayTimer>();
    stream.disconnect_and_reset();
    timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
}

void Client::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt ) return;
    printf("Client::EvtSigInt (signum=%d, sicode=%d)\n", 
        pevt.get()->signo, pevt.get()->sicode);
    timers.stopTimer<EvtEndTimer>();
    stream.disconnect_and_reset();
    shutdown();
}

