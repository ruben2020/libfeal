//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include "feal.h"

int  feal::BaseStream::accept_new_conn(void){return FEAL_HANDLE_ERROR;}
void feal::BaseStream::client_read_avail(handle_t client_sockfd){(void)(client_sockfd);}
void feal::BaseStream::client_write_avail(handle_t client_sockfd){(void)(client_sockfd);}
void feal::BaseStream::client_shutdown(handle_t client_sockfd){(void)(client_sockfd);}
void feal::BaseStream::server_shutdown(void){}
void feal::BaseStream::connected_to_server(handle_t fd){(void)(fd);}
void feal::BaseStream::connection_read_avail(void){}
void feal::BaseStream::connection_write_avail(void){}
void feal::BaseStream::connection_shutdown(void){}

void feal::BaseStream::serverLoop(void)
{
    int nfds = 0;
    handle_t tempsockfd;
    int ret;
    struct timeval tv;
    char buf[100];
    tv.tv_sec = 0;
    tv.tv_usec = 500000; // 500ms
    FD_SET ReadSet;
    FD_SET WriteSet;
    for (int j=0; j < max_events; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
    }
    sockread[0] = sockfd;
    for (;;)
    {
        if (sockfd == INVALID_SOCKET) break;
        FD_ZERO(&ReadSet);
        FD_ZERO(&WriteSet);
        for (int j=0; j < max_events; j++)
        {
            if (sockread[j]  != INVALID_SOCKET) FD_SET(sockread[j],  &ReadSet);
            if (sockwrite[j] != INVALID_SOCKET) FD_SET(sockwrite[j], &WriteSet);
        }
        nfds = select(0, &ReadSet, &WriteSet, nullptr, &tv);
        //printf("select serverloop nfds=%d\n", nfds);
        if (nfds == 0) continue; // timeout
        if (nfds == SOCKET_ERROR)
        {
            printf("select serverloop nfds=%d, err=%d\n", nfds, WSAGetLastError());
            break;
        }
        for (int i = 0; i < max_events; i++)
        {
            if (nfds <= 0) break;
            if ((i == 0) && (FD_ISSET(sockfd, &ReadSet)))
            {
                nfds--;
                if (accept_new_conn() == SOCKET_ERROR)
                {
                    do_full_shutdown();
                    server_shutdown();
                    break;
                }
                continue;
            }
            if ((i > 0) && (FD_ISSET(sockread[i], &ReadSet)))
            {
                nfds--;
                ret = recv(sockread[i], buf, sizeof(buf), MSG_PEEK);
                if ((ret == 0)||(ret == SOCKET_ERROR))
                {
                    tempsockfd = sockread[i];
                    do_client_shutdown(tempsockfd);
                    client_shutdown(tempsockfd);
                    continue;
                }
                else
                {
                    //printf("client read avail %ld\n", (long int) sockread[i]);
                    tempsockfd = sockread[i];
                    sockread[i] = INVALID_SOCKET;
                    client_read_avail(tempsockfd);
                }
            }
            if (FD_ISSET(sockwrite[i], &WriteSet))
            {
                nfds--;
                client_write_avail(sockwrite[i]);
                sockwrite[i] = INVALID_SOCKET;
            }
        }
    }
}

int feal::BaseStream::do_client_read_start(feal::handle_t client_sockfd)
{
    //printf("do_client_read_start %ld\n", (long int) client_sockfd);
    for (int i=1; i < max_events; i++)
    {
        if (sockread[i] == client_sockfd) break;
        if (sockread[i] == INVALID_SOCKET)
        {
            sockread[i] = client_sockfd;
            break;
        }
    }
    return 0;
}

int feal::BaseStream::do_client_shutdown(feal::handle_t client_sockfd)
{
    //printf("do_client_shutdown %ld\n", (long int) client_sockfd);
    for (int i=1; i < max_events; i++)
    {
        if (sockread[i] == client_sockfd)
        {
            sockread[i] = INVALID_SOCKET;
            break;
        }
    }
    int ret = shutdown(client_sockfd, SD_BOTH);
    closesocket(client_sockfd);
    return ret;
}

int feal::BaseStream::do_full_shutdown(void)
{
    int ret = 0;
    for (int j=1; j < max_events; j++)
    {
        if (sockread[j] != INVALID_SOCKET)
        {
            shutdown(sockread[j], SD_BOTH);
            closesocket(sockread[j]);
        }
        sockread[j] = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
        sockexcpt[j] = INVALID_SOCKET;
    }
    sockread[0] = INVALID_SOCKET;
    sockwrite[0] = INVALID_SOCKET;
    sockexcpt[0] = INVALID_SOCKET;
    ret = shutdown(sockfd, SD_BOTH);
    closesocket(sockfd);
    sockfd = INVALID_SOCKET;
    waitingforconn = false;
    return ret;
}

void feal::BaseStream::do_connect_in_progress(void)
{
    for (int j=0; j < max_events; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
        sockexcpt[j] = INVALID_SOCKET;
    }
    waitingforconn = true;
    sockread[0] = sockfd;
    sockwrite[0] = sockfd;
    sockexcpt[0] = sockfd;
}

void feal::BaseStream::do_connect_ok(void)
{
    for (int j=0; j < max_events; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
        sockexcpt[j] = INVALID_SOCKET;
    }
    waitingforconn = false;
    sockread[0] = sockfd;
    sockexcpt[0] = sockfd;
    connected_to_server(sockfd);
}

void feal::BaseStream::do_send_avail_notify(feal::handle_t fd)
{
    printf("do_send_avail_notify %ld\n", (long int) fd);
    for (int i=1; i < max_events; i++)
    {
        if (sockwrite[i] == INVALID_SOCKET)
        {
            sockwrite[i] = fd;
            break;
        }
    }
}

void feal::BaseStream::connectLoop(void)
{
    int nfds = 0;
    int ret;
    struct timeval tv;
    char buf[100];
    tv.tv_sec = 0;
    tv.tv_usec = 500000; // 500ms
    FD_SET ReadSet;
    FD_SET WriteSet;
    FD_SET ExceptSet;
    sockread[0] = sockfd;
    sockexcpt[0] = sockfd;
    for (;;)
    {
        if (sockfd == INVALID_SOCKET) break;
        FD_ZERO(&ReadSet);
        FD_ZERO(&WriteSet);
        FD_ZERO(&ExceptSet);
        for (int j=0; j < 3; j++)
        {
            if (sockread[j]  != INVALID_SOCKET) FD_SET(sockread[j],  &ReadSet);
            if (sockwrite[j] != INVALID_SOCKET) FD_SET(sockwrite[j], &WriteSet);
            if (sockexcpt[j] != INVALID_SOCKET) FD_SET(sockexcpt[j], &ExceptSet);
        }
        nfds = select(0, &ReadSet, &WriteSet, &ExceptSet, &tv);
        //printf("select connectloop nfds=%d\n", nfds);
        if (nfds == 0) continue; // timeout
        if (nfds == SOCKET_ERROR)
        {
            int wsaerr = WSAGetLastError();
            if (wsaerr == WSAEINVAL)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            printf("select connectloop nfds=%d, err=%d\n", nfds, wsaerr);
            break;
        }
        for (int i = 0; i < max_events; i++)
        {
            if (nfds <= 0) break;
            if (FD_ISSET(sockread[i], &ReadSet))
            {
                nfds--;
                ret = recv(sockread[i], buf, sizeof(buf), MSG_PEEK);
                if ((ret == 0)||(ret == SOCKET_ERROR))
                {
                    do_full_shutdown();
                    connection_shutdown();
                    continue;
                }
                else
                {
                    //printf("connection read avail %ld\n", (long int) sockread[i]);
                    sockread[i] = INVALID_SOCKET;
                    connection_read_avail();
                    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            if (FD_ISSET(sockwrite[i], &WriteSet))
            {
                nfds--;
                if (waitingforconn)
                {
                    waitingforconn = false;
                    connected_to_server(sockwrite[i]);
                }
                else
                {
                    connection_write_avail();
                }
                sockwrite[i] = INVALID_SOCKET;
            }
            if (FD_ISSET(sockexcpt[i], &ExceptSet))
            {
                do_full_shutdown();
                connection_shutdown();
            }
        }
    }
}
