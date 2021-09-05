#include <stdio.h>
#include "feal.h"
#include "clienthandler.h"
#include "server.h"

#define MIN(a,b) (a<b ? a : b)

feal::EventId_t EvtClientDisconnected::getId(void)
{
    return getIdOfType<EvtClientDisconnected>();
}


void ClientHandler::setParam(feal::Stream<Server>* p, feal::socket_t fd)
{
    stream = p;
    sockfd = fd;
}

void ClientHandler::initActor(void)
{
    printf("ClientHandler(%d)::initActor\n", sockfd);
}

void ClientHandler::startActor(void)
{
    printf("ClientHandler(%d)::startActor\n", sockfd);
    feal::sockerrenum se =feal::S_OK;
    if (stream) se = stream->recv_start(this, sockfd);
    if (se != feal::S_OK)
    {
        printf("Error3 %d\n", se);
    }
}

void ClientHandler::pauseActor(void)
{
    printf("ClientHandler(%d)::pauseActor\n", sockfd);
}

void ClientHandler::shutdownActor(void)
{
    printf("ClientHandler(%d)::shutdownActor\n", sockfd);
    stream->disconnect_client(sockfd);
}

void ClientHandler::handleEvent(std::shared_ptr<feal::EvtDataReadAvail> pevt)
{
    if ((!pevt)||(!stream)) return;
    printf("ClientHandler(%d)::EvtDataReadAvail\n", sockfd);
    char buf[30];
    memset(&buf, 0, sizeof(buf));
    int32_t bytes;
    feal::sockerrenum se = stream->recv_sock((void*) buf, sizeof(buf), &bytes, sockfd);
    if (se != feal::S_OK) printf("Error receiving: %d\n", se);
    else
    {
        printf("Received %d bytes: \"%s\"\n", bytes, buf);
        se = stream->send_sock((void*) buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes, sockfd);
        if (se != feal::S_OK) printf("Error sending \"ClientHandler n\": %d\n", se);
    }
}

void ClientHandler::handleEvent(std::shared_ptr<feal::EvtDataWriteAvail> pevt)
{
    if (!pevt) return;
    printf("ClientHandler(%d)::EvtDataWriteAvail\n", sockfd);
}
void ClientHandler::handleEvent(std::shared_ptr<feal::EvtClientShutdown> pevt)
{
    if (!pevt) return;
    printf("ClientHandler(%d)::EvtClientShutdown\n", sockfd);
    std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
    pevt2.get()->client_sockfd = sockfd;
    publishEvent(pevt2);
}


