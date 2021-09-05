#include <cstdio>
#include <cstring>
#include "clienthandler.h"
#include "server.h"

class EvtClientDisconnected;

feal::EventId_t EvtEndTimer::getId(void)
{
    return getIdOfType<EvtEndTimer>();
}

void Server::initActor(void)
{
    printf("Server::initActor\n");
    timers.init(this);
    stream.init(this);
    subscribeEvent<EvtClientDisconnected>(this);
}

void Server::startActor(void)
{
    printf("Server::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(80));\
    start_server();
}

void Server::pauseActor(void)
{
    printf("Server::pauseActor\n");
}

void Server::shutdownActor(void)
{
    printf("Server::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    for (auto it=mapch.begin(); it!=mapch.end(); ++it)
    {
        it->second.get()->shutdown();
    }
    mapch.clear();
    stream.disconnect_and_reset();
}

void Server::start_server(void)
{
    feal::ipaddr Serveraddr;
    Serveraddr.family = feal::ipaddr::INET;
    Serveraddr.port = 11001;
    strcpy(Serveraddr.addr, "127.0.0.1");
    printf("Starting Server on 127.0.0.1:11001\n");
    feal::sockerrenum se = stream.create_and_bind(&Serveraddr);
    if (se != feal::S_OK)
    {
        printf("Error binding to 127.0.0.1:11001  err %d\n", se);
    }
    se = stream.listen_sock();
    if (se != feal::S_OK)
    {
        printf("Error listening to 127.0.0.1:11001  err %d\n", se);
    }
    printf("Listening ...\n");
}

void Server::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtEndTimer Elapsed\n");
    shutdown();
}

void Server::handleEvent(std::shared_ptr<feal::EvtIncomingConn> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtIncomingConn\n");
    printf("Incoming connection, client socket: %d\n", pevt.get()->client_sockfd);
    if (pevt.get()-> errnum != feal::S_OK)
    {
    	printf("Error1 %d\n", pevt.get()-> errnum);
    	return;
    }
    auto it = mapch.find(pevt.get()->client_sockfd);
    if (it == mapch.end())
    {
        std::shared_ptr<ClientHandler> ch1 = std::make_shared<ClientHandler>();
        ch1.get()->setParam(&stream, pevt.get()->client_sockfd);
        ch1.get()->init();
        ch1.get()->start();
        mapch[pevt.get()->client_sockfd] = ch1;
        print_client_address(pevt.get()->client_sockfd);
    }
}

void Server::print_client_address(feal::socket_t fd)
{
    feal::sockerrenum se = feal::S_OK;
    feal::ipaddr fa;
    se = stream.getpeername_sock(&fa, fd);
    if (se == feal::S_OK)
    {
        printf("ClientHandler(%d): %s addr %s port %d\n",
            fd, (fa.family == feal::ipaddr::INET ? "IPv4" : "IPv6"), fa.addr, fa.port);
    }
    else if (se != feal::S_ENOTCONN)
    {
        printf("Error2 %d, fd=%d\n", se, fd);
    }
}

void Server::handleEvent(std::shared_ptr<feal::EvtServerShutdown> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtServerShutdown\n");
}

void Server::handleEvent(std::shared_ptr<EvtClientDisconnected> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtClientDisconnected\n");
    print_client_address(pevt.get()->client_sockfd);
    auto it = mapch.find(pevt.get()->client_sockfd);
    if (it != mapch.end())
    {
        it->second.get()->shutdown();
    }
    mapch.erase(pevt.get()->client_sockfd);
}
