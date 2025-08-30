//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

int feal::BaseStream::acceptNewConn(void)
{
    return FEAL_HANDLE_ERROR;
}
void feal::BaseStream::clientReadAvail(handle_t client_sockfd)
{
    (void)(client_sockfd);
}
void feal::BaseStream::clientWriteAvail(handle_t client_sockfd)
{
    (void)(client_sockfd);
}
void feal::BaseStream::clientShutdown(handle_t client_sockfd)
{
    (void)(client_sockfd);
}
void feal::BaseStream::serverShutdown(void) {}
void feal::BaseStream::connectedToServer(handle_t fd)
{
    (void)(fd);
}
void feal::BaseStream::connectionReadAvail(void) {}
void feal::BaseStream::connectionWriteAvail(void) {}
void feal::BaseStream::connectionShutdown(void) {}

void feal::BaseStream::serverLoop(void)
{
    int nfds = 0;
    struct epoll_event events[FEALBASESTREAM_MAXEVENTS];
    epfd = epoll_create(1);
    if (epollCtlAdd(epfd, sockfd, (EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLOUT)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl_add error");
        return;
    }
    for (;;)
    {
        nfds = epoll_wait(epfd, events, FEALBASESTREAM_MAXEVENTS, 500);
        if (nfds == -1)
        {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == sockfd)
            {
                if (((events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0) ||
                    (acceptNewConn() == -1))
                {
                    doFullShutdown();
                    serverShutdown();
                    break;
                }
                continue;
            }
            else if ((events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0)
            {
                doClientShutdown(events[i].data.fd);
                clientShutdown(events[i].data.fd);
                continue;
            }
            if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                clientReadAvail(events[i].data.fd);
            }
            if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                clientWriteAvail(events[i].data.fd);
                /*epoll_ctl_mod(epfd, events[i].data.fd,
                    (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));*/
            }
        }
    }
}

int feal::BaseStream::doClientReadStart(feal::handle_t client_sockfd)
{
    return epollCtlAdd(epfd, client_sockfd, (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT));
}

int feal::BaseStream::doClientShutdown(feal::handle_t client_sockfd)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, client_sockfd, nullptr);
    return shutdown(client_sockfd, SHUT_RDWR);
}

int feal::BaseStream::doFullShutdown(void)
{
    int res = 0;
    if (epfd != -1)
    {
        epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, nullptr);
        close(epfd);
    }
    if (sockfd != -1)
        res = shutdown(sockfd, SHUT_RDWR);
    epfd = -1;
    sockfd = -1;
    waitingforconn = false;
    return res;
}

void feal::BaseStream::doConnectInProgress(void)
{
    epfd = epoll_create(1);
    waitingforconn = true;
    if (epollCtlAdd(epfd, sockfd, (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
}

void feal::BaseStream::doConnectOk(void)
{
    epfd = epoll_create(1);
    waitingforconn = false;
    if (epollCtlAdd(epfd, sockfd, (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
    connectedToServer(sockfd);
}

void feal::BaseStream::doSendAvailNotify(feal::handle_t fd)
{
    if (epollCtlMod(epfd, fd, (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
}

void feal::BaseStream::connectLoop(void)
{
    int nfds = 0;
    struct epoll_event events[FEALBASESTREAM_MAXEVENTS];
    for (;;)
    {
        nfds = epoll_wait(epfd, events, FEALBASESTREAM_MAXEVENTS, 500);
        if (nfds == -1)
        {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        for (int i = 0; i < nfds; i++)
        {
            if ((events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0)
            {
                doFullShutdown();
                connectionShutdown();
                break;
            }
            if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                connectionReadAvail();
            }
            if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                if (waitingforconn)
                {
                    waitingforconn = false;
                    connectedToServer(events[i].data.fd);
                }
                else
                {
                    connectionWriteAvail();
                }
                /*epoll_ctl_mod(epfd, sockfd,
                    (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));*/
            }
        }
    }
}
