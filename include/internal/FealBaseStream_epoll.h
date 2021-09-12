#ifndef _FEAL_BASESTREAM_EPOLL_H
#define _FEAL_BASESTREAM_EPOLL_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <sys/epoll.h>

namespace feal
{

template<typename Y>
class BaseStream : public Tool
{
public:
BaseStream() = default;
BaseStream( const BaseStream & ) = default;
BaseStream& operator= ( const BaseStream & ) = default;
~BaseStream() = default;

protected:

socket_t sockfd = -1;
int epfd = -1;
bool waitingforconn = false;
Y* actorptr = nullptr;

virtual int  accept_new_conn(void){return -1;}
virtual void client_read_avail(int client_sockfd){(void)(client_sockfd);}
virtual void client_write_avail(int client_sockfd){(void)(client_sockfd);}
virtual void client_shutdown(int client_sockfd){(void)(client_sockfd);}
virtual void server_shutdown(void){}
virtual void connected_to_server(int fd){(void)(fd);}
virtual void connection_read_avail(void){}
virtual void connection_write_avail(void){}
virtual void connection_shutdown(void){}

void serverLoop(void)
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
            else if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                client_read_avail(events[i].data.fd);
                continue;
            }
            else if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                client_write_avail(events[i].data.fd);
                epoll_ctl_mod(epfd, events[i].data.fd, 
                    (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));
                continue;
            }
        }
    }
}

int do_client_read_start(socket_t client_sockfd)
{
    return epoll_ctl_add(epfd, client_sockfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));
}

int do_client_shutdown(socket_t client_sockfd)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, client_sockfd, NULL);
    return shutdown(client_sockfd, SHUT_RDWR);
}

int do_full_shutdown(void)
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

void do_connect_in_progress(void)
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

void do_connect_ok(void)
{
    epfd = epoll_create(1);
    if (epoll_ctl_add(epfd, sockfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
    connected_to_server(sockfd);
}

void do_send_avail_notify(int fd)
{
    if (epoll_ctl_mod(epfd, fd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
}

void connectLoop(void)
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
            else if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                connection_read_avail();
                continue;
            }
            else if ((events[i].events & EPOLLOUT) == EPOLLOUT)
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
                continue;
            }
        }
    }
}

private:

const unsigned int max_events = 64;

static int epoll_ctl_add(int epfd, socket_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

static int epoll_ctl_mod(int epfd, socket_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}


};



} // namespace feal



#endif // _FEAL_BASESTREAM_EPOLL_H
