//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "client.h"

#define SERVERPORT 57101
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
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Initial Hello from Client");
}

void Client::startActor(void)
{
    printf("Client::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(10));
    if (setup_sslctx() > 0) connect_to_server();
    else shutdown();
}

void Client::pauseActor(void)
{
    printf("Client::pauseActor\n");
}

void Client::shutdownActor(void)
{
    printf("Client::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    if (ssl) SSL_free(ssl);
    if (ctx) SSL_CTX_free(ctx);
    ssl = nullptr;
    bio = nullptr;
    ctx = nullptr;
    stream.disconnect_and_reset();
    printf("Client shutdown complete\n");
}

void Client::connect_to_server(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    feal::sockaddr_all sall;
    memset(&sall, 0, sizeof(sall));
    sall.in.sin_family = AF_INET;
    sall.in.sin_port = htons(SERVERPORT);
    feal::inet_pton(AF_INET, "127.0.0.1", &(sall.in.sin_addr));
    printf("Trying to connect to 127.0.0.1:%d\n", SERVERPORT);
    feal::errenum se = stream.connect(fd, &sall, sizeof(sall));
    if (se != feal::FEAL_OK)
    {
        printf("Error connecting to 127.0.0.1:%d  err %d\n", SERVERPORT, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}

void Client::send_something(void)
{
    perform_write(1);
}

void Client::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtEndTimer Elapsed\n");
    perform_sslshutdown();
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
        bio = nullptr;
    }
    SSL_set_bio(ssl, bio, bio);
    SSL_set_connect_state(ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    SSL_set_read_ahead(ssl, 1);
    perform_sslconnect();
}

void Client::handleEvent(std::shared_ptr<EvtDataReadAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDataReadAvail\n");
    if (sslconnect_pending)
    {
        perform_sslconnect();
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

void Client::handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDataWriteAvail\n");
    if (sslconnect_pending)
    {
        perform_sslconnect();
        return;
    }
    if (sslread_want_write)
    {
        perform_read();
    }
    if (sslwrite_want_write)
    {
        perform_write(1);
    }
    else
    {
        perform_write(0);
    }
}

void Client::handleEvent(std::shared_ptr<EvtConnectionShutdown> pevt)
{
    if (!pevt) return;
    printf("Client::EvtConnectionShutdown\n");
    timers.stopTimer<EvtDelayTimer>();
    stream.disconnect_and_reset();
    if (ssl) SSL_free(ssl);
    ssl = nullptr;
    bio = nullptr;
    timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
}

void Client::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt ) return;
    printf("Client::EvtSigInt (signum=%d, sicode=%d)\n", 
        pevt.get()->signo, pevt.get()->sicode);
    timers.stopTimer<EvtEndTimer>();
    perform_sslshutdown();
    }

int Client::perform_sslconnect(void)
{
    int ret;
    sslconnect_pending = false;
    if (ssl == nullptr)
    {
        printf("ssl not a valid pointer\n");
        return 0;
    }
    ret = SSL_connect(ssl);
    if (ret <= 0)
    {
        if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_READ)
        {
            printf("SSL_connect error SSL_ERROR_WANT_READ\n");
            sslconnect_pending = true;
        }
        else if (SSL_get_error(ssl, ret) == SSL_ERROR_WANT_WRITE)
        {
            printf("SSL_connect error SSL_ERROR_WANT_WRITE\n");
            sslconnect_pending = true;
        }
        else
        {
            printf("Fatal error performing SSL handshake with server\n");
            if (ssl) SSL_free(ssl);
            ssl = nullptr;
            bio = nullptr;
            shutdown();
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
        printf("Received %lld bytes \"%s\"\n", (long long int) bytes, buf);
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
        else if (SSL_get_error(ssl, ret) == SSL_ERROR_ZERO_RETURN)
        {
            printf("SSL_read_ex error SSL_ERROR_ZERO_RETURN\n");
            perform_sslshutdown();
        }
        else
        {
            printf("Fatal error doing SSL_read_ex\n");
            perform_sslshutdown();
            shutdown();
        }
    }
    return ret;
}

int Client::perform_write(int num)
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
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Client %d", n);
    if (num != 0) n++;
    sslwrite_want_read = false;
    sslwrite_want_write = false;
    if (num == 0) ret = SSL_write_ex(ssl, buf, 0, &bytes);
    else ret = SSL_write_ex(ssl, buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes);
    printf("ret value SSL_write_ex = %d\n", ret);
    if (ret > 0)
    {
        if (bytes > 0)
        printf("Sent %lld bytes \"%s\"\n", (long long int) bytes, buf);
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
            shutdown();
        }
    }
    return ret;
}

int Client::perform_sslshutdown(void)
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
        shutdown();
    }
    else if (ret == 0)
    {
        sslshutdown_pending = true;
        printf("SSL_shutdown pending, ret = 0\n");
    }
    else if (ret <= -1)
    {
        printf("SSL_shutdown failed, ret = -1\n");
        shutdown();
    }
    return ret;
}

int Client::setup_sslctx(void)
{
    // Source code copied from:
    // https://github.com/openssl/openssl/blob/master/demos/guide/tls-client-block.c

    ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == nullptr)
    {
        printf("Failed to create client SSL_CTX\n");
        return -1;
    }

    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
    {
        SSL_CTX_free(ctx);
        printf("Failed to set the minimum TLS protocol version\n");
        return -1;
    }
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if (SSL_CTX_load_verify_file(ctx, "server.crt") <= 0) 
    {
        SSL_CTX_free(ctx);
        printf("Failed to load the certificate chain file\n");
        return -1;
    }
    return 1;

}

