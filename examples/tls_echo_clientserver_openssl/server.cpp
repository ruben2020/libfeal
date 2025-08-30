//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "server.h"

#include <cstdio>
#include <cstring>

#include "clienthandler.h"

#define SERVERPORT 57101

static const unsigned char cache_id[] = "libfeal TLS Demo Server";

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
    if (setupSslctx() > 0)
        startServer();
    else
        shutdown();
}

void Server::pauseActor(void)
{
    printf("Server::pauseActor\n");
}

void Server::shutdownActor(void)
{
    printf("Server::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    for (auto it = mapch.begin(); it != mapch.end(); ++it)
    {
        it->second.get()->shutdown();
    }
    if (ctx)
        SSL_CTX_free(ctx);
    mapch.clear();
    stream.disconnectAndReset();
}

void Server::startServer(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    feal::sockaddr_all_t sall;
    memset(&sall, 0, sizeof(sall));
    sall.in.sin_family = AF_INET;
    sall.in.sin_port = htons(SERVERPORT);
    feal::inet_pton(AF_INET, "127.0.0.1", &(sall.in.sin_addr));
    feal::setReuseAddr(fd, true);
    printf("Starting Server on 127.0.0.1:%d\n", SERVERPORT);
    int ret = bind(fd, (sockaddr*)&(sall.in), sizeof(sall.in));
    if (ret != feal::FEAL_OK)
    {
        printf("Error binding to 127.0.0.1:%d  err %d\n", SERVERPORT,
               static_cast<feal::errenum_t>(FEAL_GETHANDLEERRNO));
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    feal::errenum_t se = stream.listen(fd);
    if (se != feal::FEAL_OK)
    {
        printf("Error listening to 127.0.0.1:%d  err %d\n", SERVERPORT, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Listening ...\n");
}

void Server::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtEndTimer Elapsed\n");
    std::shared_ptr<EvtClientSSLShutdown> pevt2 = std::make_shared<EvtClientSSLShutdown>();
    publishEvent(pevt2);
}

void Server::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtRetryTimer Elapsed\n");
    startServer();
}

void Server::handleEvent(std::shared_ptr<EvtIncomingConn> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtIncomingConn\n");
    printf("Incoming connection, client socket: %lld\n", (long long int)pevt.get()->fd);
    if (pevt.get()->errnum != feal::FEAL_OK)
    {
        printf("Error1 %d\n", pevt.get()->errnum);
        return;
    }
    auto it = mapch.find(pevt.get()->fd);
    if (it == mapch.end())
    {
        char buf[100];
        getClientAddress(pevt.get()->fd, buf, sizeof(buf));
        std::shared_ptr<ClientHandler> ch1 = std::make_shared<ClientHandler>();
        ch1.get()->setParam(&stream, pevt.get()->fd, buf, ctx);
        ch1.get()->init();
        ch1.get()->start();
        mapch[pevt.get()->fd] = ch1;
        printClientAddress(pevt.get()->fd);
    }
}

void Server::printClientAddress(feal::handle_t fd)
{
    feal::sockaddr_all_t sall;
    feal::socklen_t length = sizeof(sall);
    int ret = ::getpeername(fd, &(sall.sa), &length);
    feal::errenum_t se;
    if (ret != feal::FEAL_OK)
        se = static_cast<feal::errenum_t>(FEAL_GETHANDLEERRNO);
    if (ret == feal::FEAL_OK)
    {
        printf("ClientHandler(%lld): %s addr %s port %s\n", (long long int)fd,
               (sall.sa.sa_family == AF_INET ? "IPv4" : "IPv6"), feal::getAddr(&sall).c_str(),
               feal::getPort(&sall).c_str());
    }
    else if ((se != feal::FEAL_ENOTCONN) && (se != feal::FEAL_ENOTSOCK))
    {
        printf("Error2 %d, fd=%lld\n", se, (long long int)fd);
    }
}

void Server::getClientAddress(feal::handle_t fd, char* addr, int addrbuflen)
{
    feal::sockaddr_all_t sall;
    feal::socklen_t length = sizeof(sall);
    int ret = ::getpeername(fd, &(sall.sa), &length);
    if ((ret == feal::FEAL_OK) && (addr))
    {
        snprintf(addr, addrbuflen, "%s %s port %s",
                 (sall.sa.sa_family == AF_INET ? "IPv4" : "IPv6"), feal::getAddr(&sall).c_str(),
                 feal::getPort(&sall).c_str());
    }
}

void Server::handleEvent(std::shared_ptr<EvtServerShutdown> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtServerShutdown\n");
}

void Server::handleEvent(std::shared_ptr<EvtClientDisconnected> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtClientDisconnected\n");
    printClientAddress(pevt.get()->fd);
    auto it = mapch.find(pevt.get()->fd);
    if (it != mapch.end())
    {
        it->second.get()->shutdown();
    }
    mapch.erase(pevt.get()->fd);
    if (mapch.empty())
        shutdown();
}

void Server::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtSigInt (signum=%d, sicode=%d)\n", pevt.get()->signo, pevt.get()->sicode);
    timers.stopTimer<EvtEndTimer>();
    std::shared_ptr<EvtClientSSLShutdown> pevt2 = std::make_shared<EvtClientSSLShutdown>();
    publishEvent(pevt2);
}

int Server::setupSslctx(void)
{
    // Source code copied from:
    // https://github.com/openssl/openssl/blob/master/demos/guide/tls-server-block.c

    long opts;
    ctx = SSL_CTX_new(TLS_server_method());
    if (ctx == nullptr)
    {
        printf("Failed to create server SSL_CTX\n");
        return -1;
    }

    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
    {
        SSL_CTX_free(ctx);
        printf("Failed to set the minimum TLS protocol version\n");
        return -1;
    }
    opts = SSL_OP_IGNORE_UNEXPECTED_EOF;
    opts |= SSL_OP_NO_RENEGOTIATION;
    opts |= SSL_OP_CIPHER_SERVER_PREFERENCE;
    SSL_CTX_set_options(ctx, opts);

    if (SSL_CTX_use_certificate_chain_file(ctx, "server.crt") <= 0)
    {
        SSL_CTX_free(ctx);
        printf("Failed to load the server certificate chain file\n");
        return -1;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0)
    {
        SSL_CTX_free(ctx);
        printf("Error loading the server private key file"
               "possible key/cert mismatch???\n");
        return -1;
    }

    SSL_CTX_set_session_id_context(ctx, cache_id, sizeof(cache_id));
    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_SERVER);
    SSL_CTX_sess_set_cache_size(ctx, 1024);
    SSL_CTX_set_timeout(ctx, 3600);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
    return 1;
}
