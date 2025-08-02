//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "clienthandler.h"
#include "server.h"


void Server::initActor(void)
{
    printf("Server::initActor\n");
    timers.init(this);
    stream.init(this);
    stream.subscribeIncomingConn<EvtIncomingConn>();
    stream.subscribeServerShutdown<EvtServerShutdown>();
    signal.init(this);
    subscribeEvent<EvtClientDisconnected>(this);
    signal.registerSignal<EvtSigInt>(SIGINT);
}

void Server::startActor(void)
{
    printf("Server::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(15));
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
    feal::ipaddr serveraddr;
    serveraddr.family = feal::ipaddr::INET;
    serveraddr.port = 11001;
    strcpy(serveraddr.addr, "127.0.0.1");
    printf("Starting Server on 127.0.0.1:11001\n");
    feal::errenum se = stream.create_and_bind(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("Error binding to 127.0.0.1:11001  err %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    se = stream.listen();
    if (se != feal::FEAL_OK)
    {
        printf("Error listening to 127.0.0.1:11001  err %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Listening ...\n");
}

void Server::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtEndTimer Elapsed\n");
    shutdown();
}

void Server::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtRetryTimer Elapsed\n");
    start_server();
}

void Server::handleEvent(std::shared_ptr<EvtIncomingConn> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtIncomingConn\n");
    printf("Incoming connection, client socket: %ld\n", (long int) pevt.get()->fd);
    if (pevt.get()-> errnum != feal::FEAL_OK)
    {
    	printf("Error1 %d\n", pevt.get()-> errnum);
    	return;
    }
    auto it = mapch.find(pevt.get()->fd);
    if (it == mapch.end())
    {
        char buf[100];
        get_client_address(pevt.get()->fd, buf, sizeof(buf));
        std::shared_ptr<ClientHandler> ch1 = std::make_shared<ClientHandler>();
        ch1.get()->setParam(&stream, pevt.get()->fd, buf);
        ch1.get()->init();
        ch1.get()->start();
        mapch[pevt.get()->fd] = ch1;
        print_client_address(pevt.get()->fd);
    }
}

void Server::print_client_address(feal::handle_t fd)
{
    feal::errenum se = feal::FEAL_OK;
    feal::ipaddr fa;
    se = stream.getpeername(&fa, fd);
    if (se == feal::FEAL_OK)
    {
        printf("ClientHandler(%ld): %s addr %s port %d\n",
            (long int) fd, (fa.family == feal::ipaddr::INET ? "IPv4" : "IPv6"), fa.addr, fa.port);
    }
    else if ((se != feal::FEAL_ENOTCONN)&&(se != feal::FEAL_ENOTSOCK))
    {
        printf("Error2 %d, fd=%ld\n", se, (long int) fd);
    }
}

void Server::get_client_address(feal::handle_t fd, char* addr, int addrbuflen)
{
    feal::errenum se = feal::FEAL_OK;
    feal::ipaddr fa;
    se = stream.getpeername(&fa, fd);
    if ((se == feal::FEAL_OK)&&(addr))
    {
        snprintf(addr, addrbuflen, "%s %s port %d",
            (fa.family == feal::ipaddr::INET ? "IPv4" : "IPv6"), fa.addr, fa.port);
    }
}

void Server::handleEvent(std::shared_ptr<EvtServerShutdown> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtServerShutdown\n");
}

void Server::handleEvent(std::shared_ptr<EvtClientDisconnected> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtClientDisconnected\n");
    print_client_address(pevt.get()->fd);
    auto it = mapch.find(pevt.get()->fd);
    if (it != mapch.end())
    {
        it->second.get()->shutdown();
    }
    mapch.erase(pevt.get()->fd);
}

void Server::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt ) return;
    printf("Server::EvtSigInt (signum=%d, sicode=%d)\n", 
        pevt.get()->signo, pevt.get()->sicode);
    timers.stopTimer<EvtEndTimer>();
    shutdown();
}
