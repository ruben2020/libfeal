//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

int feal::BaseStream::accept_new_conn(void)
{
    return FEAL_HANDLE_ERROR;
}
void feal::BaseStream::client_read_avail(handle_t client_sockfd)
{
    (void)(client_sockfd);
}
void feal::BaseStream::client_write_avail(handle_t client_sockfd)
{
    (void)(client_sockfd);
}
void feal::BaseStream::client_shutdown(handle_t client_sockfd)
{
    (void)(client_sockfd);
}
void feal::BaseStream::server_shutdown(void) {}
void feal::BaseStream::connected_to_server(handle_t fd)
{
    (void)(fd);
}
void feal::BaseStream::connection_read_avail(void) {}
void feal::BaseStream::connection_write_avail(void) {}
void feal::BaseStream::connection_shutdown(void) {}

void feal::BaseStream::serverLoop(void)
{
    int nevts = 0;
    struct timespec tims;
    struct kevent change_event[4], event[FEALBASESTREAM_MAXEVENTS];
    memset(&change_event, 0, sizeof(change_event));
    memset(&event, 0, sizeof(event));
    tims.tv_sec = 0;
    tims.tv_nsec = 500000000;  // 500ms
    kq = kqueue();
    EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    if (kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent error");
        return;
    }
    EV_SET(change_event, sockfd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
    if (kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent error");
        return;
    }
    for (;;)
    {
        nevts = kevent(kq, nullptr, 0, event, FEALBASESTREAM_MAXEVENTS - 1,
                       (const struct timespec *)&tims);
        if (nevts == 0)
            continue;
        if (nevts == -1)
            break;
        for (int i = 0; i < nevts; i++)
        {
            if (sockfd == (int)event[i].ident)
            {
                if (((event[i].flags & (EV_EOF | EV_ERROR)) != 0) || (accept_new_conn() == -1))
                {
                    do_full_shutdown();
                    server_shutdown();
                    break;
                }
                continue;
            }
            else if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                do_client_shutdown((int)event[i].ident);
                client_shutdown((int)event[i].ident);
                continue;
            }
            if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                client_read_avail((int)event[i].ident);
            }
            if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                client_write_avail((int)event[i].ident);
                /*EV_SET(change_event, event[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
                kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);*/
            }
        }
    }
}

int feal::BaseStream::do_client_read_start(feal::handle_t client_sockfd)
{
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, sockfd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr);
    EV_SET(change_event, client_sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    return kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr);
}

int feal::BaseStream::do_client_shutdown(feal::handle_t client_sockfd)
{
    int rc = shutdown(client_sockfd, SHUT_RDWR);
    close(client_sockfd);
    return rc;
}

int feal::BaseStream::do_full_shutdown(void)
{
    int res = 0;
    if (sockfd != -1)
        res = shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    close(kq);
    kq = -1;
    sockfd = -1;
    waitingforconn = false;
    return res;
}

void feal::BaseStream::do_connect_in_progress(void)
{
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    waitingforconn = true;
    kq = kqueue();
    EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr);
    EV_SET(change_event, sockfd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr);
}

void feal::BaseStream::do_connect_ok(void)
{
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    waitingforconn = false;
    kq = kqueue();
    EV_SET(change_event, sockfd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr);
    EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("do_connect_ok err");
    }
    else
        connected_to_server(sockfd);
}

void feal::BaseStream::do_send_avail_notify(feal::handle_t fd)
{
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr);
}

void feal::BaseStream::connectLoop(void)
{
    int nevts = 0;
    struct timespec tims;
    struct kevent change_event[4], event[FEALBASESTREAM_MAXEVENTS];
    memset(&change_event, 0, sizeof(change_event));
    memset(&event, 0, sizeof(event));
    tims.tv_sec = 0;
    tims.tv_nsec = 500000000;  // 500ms
    for (;;)
    {
        nevts = kevent(kq, nullptr, 0, event, FEALBASESTREAM_MAXEVENTS - 1,
                       (const struct timespec *)&tims);
        if (nevts == 0)
            continue;
        if (nevts == -1)
            break;
        for (int i = 0; i < nevts; i++)
        {
            if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                do_full_shutdown();
                connection_shutdown();
                break;
            }
            if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                connection_read_avail();
            }
            if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                if (waitingforconn)
                {
                    waitingforconn = false;
                    connected_to_server((int)event[i].ident);
                }
                else
                {
                    connection_write_avail();
                }
                /*EV_SET(change_event, sockfd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
                kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);*/
            }
        }
    }
}
