//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "client.h"

#define MIN(a,b) (a<b ? a : b)


void Client::initActor(void)
{
    printf("Client::initActor\n");
    timers.init(this);
    stream.init(this);
    stream.subscribeConnectedToServer<EvtConnectedToServer>();
    stream.subscribeReadAvail<EvtDataReadAvail>();
    stream.subscribeWriteAvail<EvtDataWriteAvail>();
    stream.subscribeConnectionShutdown<EvtConnectionShutdown>();
    signal.init(this);
    signal.registerSignal<EvtSigInt>(SIGINT);
}

void Client::startActor(void)
{
    printf("Client::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(15));
    setup_sslctx();
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
    if ((!sslwrite_want_read)&&(!sslwrite_want_write))
        perform_write();
}

void Client::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtEndTimer Elapsed\n");
    if (ssl) SSL_free(ssl);
    if (ctx) SSL_CTX_free(ctx);
    if (bio) BIO_free(bio);
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

void Client::handleEvent(std::shared_ptr<EvtConnectedToServer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtConnectedToServer\n");
    bio = BIO_new_socket(pevt->fd, BIO_NOCLOSE);
    if ((ssl = SSL_new(ctx)) == NULL)
    {
        printf("Error creating SSL handle for new connection");
        BIO_free(bio);
    }
    SSL_set_bio(ssl, bio, bio);
    ssl_connect();
}

void Client::handleEvent(std::shared_ptr<EvtDataReadAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDataReadAvail\n");
    if (sslconnect_want_read)
    {
        ssl_connect();
    }
    else if (sslwrite_want_read)
    {
        perform_write();
    }
    else
    {
        perform_read();
    }
}

void Client::handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDataWriteAvail\n");
    if (sslconnect_want_write)
    {
        ssl_connect();
        return;
    }
    if (sslread_want_write)
    {
        perform_read();
    }
    if (sslwrite_want_write)
    {
        perform_write();
    }
}

void Client::handleEvent(std::shared_ptr<EvtConnectionShutdown> pevt)
{
    if (!pevt) return;
    printf("Client::EvtConnectionShutdown\n");
    timers.stopTimer<EvtDelayTimer>();
    if (ssl) SSL_free(ssl);
    if (bio) BIO_free(bio);
    stream.disconnect_and_reset();
    timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
}

void Client::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt ) return;
    printf("Client::EvtSigInt (signum=%d, sicode=%d)\n", 
        pevt.get()->signo, pevt.get()->sicode);
    timers.stopTimer<EvtEndTimer>();
    if (ssl) SSL_free(ssl);
    if (ctx) SSL_CTX_free(ctx);
    if (bio) BIO_free(bio);
    stream.disconnect_and_reset();
    shutdown();
}

int Client::ssl_connect(void)
{
    int ret;
    sslconnect_want_read = false;
    sslconnect_want_write = false;
    ret = SSL_connect(ssl);
    if (ret <= 0)
    {
        if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_READ)
        {
            printf("SSL_connect error SSL_ERROR_WANT_READ\n");
            sslconnect_want_read = true;
        }
        else if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_WRITE)
        {
            printf("SSL_connect error SSL_ERROR_WANT_WRITE\n");
            sslconnect_want_write = true;
        }
        else
        {
            printf("Fatal error performing SSL handshake with server\n");
            if (ssl) SSL_free(ssl);
            if (bio) BIO_free(bio);
        }
    }
    else
    {
        printf("SSL handshake with server successfully completed!\n ");
        send_something();
    }
    return ret;
}

int Client::perform_read(void)
{
    memset(&buf, 0, sizeof(buf));
    size_t bytes;
    int ret;
    sslread_want_write = false;
    ret = SSL_read_ex(ssl, buf, sizeof(buf), &bytes);
    if (ret > 0)
    {
        printf("Received %ld bytes \"%s\"\n", bytes, buf);
        timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
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
        else
        {
            printf("Fatal error doing SSL_read_ex\n");
            if (ssl) SSL_free(ssl);
            if (bio) BIO_free(bio);
        }
    }
    return ret;
}

int Client::perform_write(void)
{
    size_t bytes;
    int ret;
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Client %d", n++);
    sslwrite_want_read = false;
    sslwrite_want_write = false;
    ret = SSL_write_ex(ssl, buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes);
    if (ret > 0)
    {
        printf("Sent %ld bytes \"%s\"\n", bytes, buf);
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
            if (ssl) SSL_free(ssl);
            if (bio) BIO_free(bio);
        }
    }
    return ret;
}

void Client::setup_sslctx(void)
{
    // Source code copied from:
    // https://github.com/openssl/openssl/blob/master/demos/guide/tls-client-block.c

    ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == nullptr)
    {
        printf("Failed to create client SSL_CTX");
    }

    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
    {
        SSL_CTX_free(ctx);
        printf("Failed to set the minimum TLS protocol version");
    }
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if (SSL_CTX_load_verify_file(ctx, "servercert.pem") <= 0) 
    {
        SSL_CTX_free(ctx);
        printf("Failed to load the certificate chain file");
    }

}

