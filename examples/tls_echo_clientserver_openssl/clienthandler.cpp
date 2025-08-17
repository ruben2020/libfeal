//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <stdio.h>
#include "feal.h"
#include "clienthandler.h"
#include "server.h"

#define MIN(a,b) (a<b ? a : b)


void ClientHandler::setParam(feal::Stream<Server>* p, feal::handle_t fd, char *s, SSL_CTX *ctx1)
{
    stream = p;
    sockfd = fd;
    addrstr = s;
    ctx = ctx1;
}

void ClientHandler::initActor(void)
{
    printf("ClientHandler(%lld)::initActor\n", (long long int) sockfd);
    subscribeEvent<EvtClientSSLShutdown>(this);
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Initial Hello from Server");
    client_bio = BIO_new_socket(sockfd, BIO_NOCLOSE);
    if ((ssl = SSL_new(ctx)) == NULL)
    {
        printf("Error creating SSL handle for new connection");
        BIO_free(client_bio);
    }
    SSL_set_bio(ssl, client_bio, client_bio);
    SSL_set_accept_state(ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    SSL_set_read_ahead(ssl, 1);
}

void ClientHandler::startActor(void)
{
    printf("ClientHandler(%lld)::startActor\n", (long long int) sockfd);
    feal::errenum se =feal::FEAL_OK;
    if (stream) se = stream->registerClient<ClientHandler, EvtDataReadAvail, EvtDataWriteAvail, EvtClientShutdown>(this, sockfd);
    if (se != feal::FEAL_OK)
    {
        printf("Error3 %d\n", se);
    }
    perform_sslaccept();
}

void ClientHandler::pauseActor(void)
{
    printf("ClientHandler(%lld)::pauseActor\n", (long long int) sockfd);
}

void ClientHandler::shutdownActor(void)
{
    printf("ClientHandler(%lld)::shutdownActor\n", (long long int) sockfd);
    stream->disconnect_client(sockfd);
    if (ssl) SSL_free(ssl);
    ssl = nullptr;
    client_bio = nullptr;
}

void ClientHandler::handleEvent(std::shared_ptr<EvtDataReadAvail> pevt)
{
    printf("ClientHandler(%lld)::EvtDataReadAvail\n", (long long int) sockfd);
    if ((!pevt)||(!stream)) return;
    if (sslaccept_pending)
    {
        perform_sslaccept();
    }
    else if (sslwrite_want_read)
    {
        perform_write(1);
    }
    else
    {
        perform_read();
    }
}

void ClientHandler::handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt)
{
    if ((!pevt)||(!stream)) return;
    printf("ClientHandler(%lld)::EvtDataWriteAvail\n", (long long int) sockfd);
    if (sslaccept_pending)
    {
        perform_sslaccept();
    }
    else if (sslread_want_write)
    {
        perform_read();
    }
    else if (sslwrite_want_write)
    {
        perform_write(1);
    }
    else
    {
        perform_write(0);
    }
}

void ClientHandler::handleEvent(std::shared_ptr<EvtClientShutdown> pevt)
{
    if (!pevt) return;
    printf("ClientHandler(%lld)::EvtClientShutdown\n", (long long int) sockfd);
    std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
    pevt2.get()->fd = sockfd;
    publishEvent(pevt2);
}

void ClientHandler::handleEvent(std::shared_ptr<EvtClientSSLShutdown> pevt)
{
    if (!pevt) return;
    printf("ClientHandler(%lld)::EvtClientSSLShutdown\n", (long long int) sockfd);
    perform_sslshutdown();
}

int ClientHandler::perform_sslaccept(void)
{
    int ret;
    if (ssl == nullptr)
    {
        printf("ssl not a valid pointer\n");
        return 0;
    }
    sslaccept_pending = false;
    ret = SSL_accept(ssl);
    if (ret <= 0)
    {
        if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_READ)
        {
            printf("SSL_accept error SSL_ERROR_WANT_READ\n");
            sslaccept_pending = true;
        }
        else if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_WRITE)
        {
            printf("SSL_accept error SSL_ERROR_WANT_WRITE\n");
            sslaccept_pending = true;
        }
        else
        {
            printf("Fatal error performing SSL handshake with client\n");
            if (ssl) SSL_free(ssl);
            ssl = nullptr;
            client_bio = nullptr;
            std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
            pevt2.get()->fd = sockfd;
            publishEvent(pevt2);
       }
    }
    else printf("SSL handshake with client successfully completed!\n ");
    return ret;
}

int ClientHandler::perform_read(void)
{
    printf("perform_read\n");
    if (ssl == nullptr)
    {
        printf("ssl not a valid pointer\n");
        return 0;
    }
    memset(&buf, 0, sizeof(buf));
    size_t bytes;
    int ret = 0;
    sslread_want_write = false;
    ret = SSL_read_ex(ssl, buf, sizeof(buf), &bytes);
    printf("ret value SSL_read_ex = %d\n", ret);
    if (ret > 0)
    {
        printf("Received %lld bytes \"%s\" from %s\n", (long long int) bytes, buf, addrstr.c_str());
        perform_write(1);
    }
    else
    {
        if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_READ)
        {
            printf("SSL_read_ex error SSL_ERROR_WANT_READ\n");
        }
        else if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_WRITE)
        {
            printf("SSL_read_ex error SSL_ERROR_WANT_WRITE\n");
            sslread_want_write = true;
        }
        else if (SSL_get_error(ssl, ret) == SSL_ERROR_ZERO_RETURN)
        {
            printf("SSL_read_ex error SSL_ERROR_ZERO_RETURN\n");
            perform_sslshutdown();
        }
        else
        {
            printf("Fatal error doing SSL_read_ex\n");
            perform_sslshutdown();
            std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
            pevt2.get()->fd = sockfd;
            publishEvent(pevt2);
        }
    }
    return ret;
}

int ClientHandler::perform_write(int num)
{
    printf("perform_write(%d)\n", num);
    if (sslshutdown_pending || sslshutdown_complete)
    {
        printf("No more writing because SSL connection has been shutdown\n");
        return 0;
    }
    if (ssl == nullptr)
    {
        printf("ssl not a valid pointer\n");
        return 0;
    }
    size_t bytes;
    int ret;
    sslwrite_want_read = false;
    sslwrite_want_write = false;
    if (num == 0) ret = SSL_write_ex(ssl, buf, 0, &bytes);
    else ret = SSL_write_ex(ssl, buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes);
    printf("ret value SSL_write_ex = %d\n", ret);
    if (ret > 0)
    {
        if (bytes > 0)
        printf("Sent %lld bytes \"%s\" to %s\n", (long long int) bytes, buf, addrstr.c_str());
    }
    else
    {
        if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_READ)
        {
            printf("SSL_write_ex error SSL_ERROR_WANT_READ\n");
            sslwrite_want_read = true;
        }
        else if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_WRITE)
        {
            printf("SSL_write_ex error SSL_ERROR_WANT_WRITE\n");
            sslwrite_want_write = true;
        }
        else
        {
            printf("Fatal error doing SSL_write_ex\n");
            perform_sslshutdown();
            std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
            pevt2.get()->fd = sockfd;
            publishEvent(pevt2);
        }
    }
    return ret;
}

int ClientHandler::perform_sslshutdown(void)
{
    int ret;
    if (ssl == nullptr)
    {
        printf("ssl not a valid pointer\n");
        return 0;
    }
    sslshutdown_pending = false;
    sslshutdown_complete = false;
    ret = SSL_shutdown(ssl);
    if (ret >= 1)
    {
        sslshutdown_complete = true;
        printf("SSL_shutdown successfully completed, ret = 1\n");
        std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
        pevt2.get()->fd = sockfd;
        publishEvent(pevt2);
    }
    else if (ret == 0)
    {
        sslshutdown_pending = true;
        printf("SSL_shutdown pending, ret = 0\n");
    }
    else if (ret <= -1)
    {
        printf("SSL_shutdown failed, ret = -1\n");
        std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
        pevt2.get()->fd = sockfd;
        publishEvent(pevt2);
    }
    return ret;
}
