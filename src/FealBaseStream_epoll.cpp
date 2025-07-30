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
    struct epoll_event events[max_events];
    epfd = epoll_create(1);
    if (epoll_ctl_add(epfd, sockfd, (EPOLLIN | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl_add error");
        return;
    }
    for (;;)
    {
        nfds = epoll_wait(epfd, events, max_events, 500);
        if (nfds == -1)
        {
            if (errno == EINTR) continue;
            else break;
        }
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == sockfd)
            {
                if (((events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0)||
                    (accept_new_conn() == -1))
                {
                    do_full_shutdown();
                    server_shutdown();
                    break;
                }
                continue;
            }
            else if ((events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0)
            {
                do_client_shutdown(events[i].data.fd);
                client_shutdown(events[i].data.fd);
                continue;
            }
            if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                client_read_avail(events[i].data.fd);
            }
            if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                client_write_avail(events[i].data.fd);
                epoll_ctl_mod(epfd, events[i].data.fd, 
                    (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));
            }
        }
    }
}

int feal::BaseStream::do_client_read_start(feal::handle_t client_sockfd)
{
    return epoll_ctl_add(epfd, client_sockfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));
}

int feal::BaseStream::do_client_shutdown(feal::handle_t client_sockfd)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, client_sockfd, NULL);
    return shutdown(client_sockfd, SHUT_RDWR);
}

int feal::BaseStream::do_full_shutdown(void)
{
    int res = 0;
    if (epfd != -1)
    {
        epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
        close(epfd);
    }
    if (sockfd != -1) res = shutdown(sockfd, SHUT_RDWR);
    epfd = -1;
    sockfd = -1;
    waitingforconn = false;
    return res;
}

void feal::BaseStream::do_connect_in_progress(void)
{
    epfd = epoll_create(1);
    waitingforconn = true;
    if (epoll_ctl_add(epfd, sockfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
}

void feal::BaseStream::do_connect_ok(void)
{
    epfd = epoll_create(1);
    waitingforconn = false;
    if (epoll_ctl_add(epfd, sockfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
    connected_to_server(sockfd);
}

void feal::BaseStream::do_send_avail_notify(feal::handle_t fd)
{
    if (epoll_ctl_mod(epfd, fd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
}

void feal::BaseStream::connectLoop(void)
{
    int nfds = 0;
    struct epoll_event events[max_events];
    for (;;)
    {
        nfds = epoll_wait(epfd, events, max_events, 500);
        if (nfds == -1)
        {
            if (errno == EINTR) continue;
            else break;
        }
        for (int i = 0; i < nfds; i++)
        {
            if ((events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0)
            {
                do_full_shutdown();
                connection_shutdown();
                break;
            }
            if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                connection_read_avail();
            }
            if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                if (waitingforconn)
                {
                    waitingforconn = false;
                    connected_to_server(events[i].data.fd);
                }
                else
                {
                    connection_write_avail();
                }
                epoll_ctl_mod(epfd, sockfd, 
                    (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));
            }
        }
    }
}

#if defined (__linux__)
int feal::BaseStream::epoll_ctl_add(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

int feal::BaseStream::epoll_ctl_mod(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}
#endif
