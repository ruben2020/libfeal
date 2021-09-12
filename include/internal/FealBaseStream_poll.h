#ifndef _FEAL_BASESTREAM_POLL_H
#define _FEAL_BASESTREAM_POLL_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <sys/poll.h>
#include <string.h>

namespace feal
{

template<typename Y>
class BaseStream : public Tool
{
public:
BaseStream()
{
    memset(events, 0 , sizeof(events));
}
BaseStream( const BaseStream & ) = default;
BaseStream& operator= ( const BaseStream & ) = default;
~BaseStream() = default;

protected:

socket_t sockfd = -1;
bool waitingforconn = false;
Y* actorptr = nullptr;
struct pollfd events[64];

virtual int  accept_new_conn(void){return -1;}
virtual void client_read_avail(int client_sockfd){(void)(client_sockfd);}
virtual void client_write_avail(int client_sockfd){(void)(client_sockfd);}
virtual void client_shutdown(int client_sockfd){(void)(client_sockfd);}
virtual void server_shutdown(void){}
virtual void connected_to_server(void){}
virtual void connection_read_avail(void){}
virtual void connection_write_avail(void){}
virtual void connection_shutdown(void){}

void serverLoop(void)
{
    int nfds = 0;
    short mask;
    memset(events, 0 , sizeof(events));
    events[0].fd = sockfd;
    events[0].events = (POLLIN | POLLHUP | POLLERR);
    for (;;)
    {
        if (sockfd == -1) break;
        nfds = poll(events, max_events - 1, 500);
        //printf("nfds = %d\n", nfds);
        if (nfds ==  0) continue; // timeout
        if (nfds == -1) break; // error
        for (unsigned int i = 0; i < max_events - 1; i++)
        {
            if (nfds <= 0) break;
            if(events[i].revents == 0) continue;
            nfds--;
            if (events[i].fd == sockfd)
            {
                if (((events[i].revents & (POLLHUP | POLLERR)) != 0)||
                    (accept_new_conn() == -1))
                {
                    do_full_shutdown();
                    server_shutdown();
                    events[i].revents = 0;
                    break;
                }
                events[i].revents = 0;
                continue;
            }
            else if ((events[i].revents & (POLLHUP | POLLERR)) != 0)
            {
                shutdown(events[i].fd, SHUT_RDWR);
                events[i].fd = 0;
                events[i].events = 0;
                client_shutdown(events[i].fd);
                events[i].revents = 0;
                continue;
            }
            else if ((events[i].revents & POLLIN) == POLLIN)
            {
                client_read_avail(events[i].fd);
                mask = POLLIN;
                events[i].events &= (~mask);
                continue;
            }
            else if ((events[i].revents & POLLOUT) == POLLOUT)
            {
                client_write_avail(events[i].fd);
                mask = POLLOUT;
                events[i].events &= (~mask);
                continue;
            }
        }
    }
}

int do_client_read_start(socket_t client_sockfd)
{
    for (unsigned int i=0; i < max_events - 1; i++)    
    {
        if (events[i].fd == 0)
        {
            events[i].fd = client_sockfd;
            events[i].events = (POLLIN | POLLHUP | POLLERR);
            break;
        }
    }
    return 0;
}

int do_client_shutdown(socket_t client_sockfd)
{
    for (unsigned int i=0; i < max_events - 1; i++)    
    {
        if (events[i].fd == client_sockfd)
        {
            events[i].fd = 0;
            events[i].events = 0;
            break;
        }
    }
    return shutdown(client_sockfd, SHUT_RDWR);
}

int do_full_shutdown(void)
{
    int res = 0;
    if (sockfd != -1) res = shutdown(sockfd, SHUT_RDWR);
    memset(events, 0 , sizeof(events));
    sockfd = -1;
    waitingforconn = false;
    return res;
}

void do_connect_in_progress(void)
{
    waitingforconn = true;
    events[0].fd = sockfd;
    events[0].events = (POLLIN | POLLHUP | POLLERR | POLLOUT);
}

void do_connect_ok(void)
{
    events[0].fd = sockfd;
    events[0].events = (POLLIN | POLLHUP | POLLERR);
    connected_to_server();
}

void do_send_avail_notify(int fd)
{
    for (unsigned int i=0; i < max_events - 1; i++)    
    {
        if (events[i].fd == fd)
        {
            events[i].events |= (short) POLLOUT;
            break;
        }
    }
}

void do_recv_complete(int fd, ssize_t numbytes)
{
    //printf("fd = %d, numbytes = %zd\n", fd, numbytes);
    if (numbytes == 0)
    {
        if (fd == sockfd) shutdown(fd, SHUT_RDWR);
        else
        {
            for (unsigned int j=0; j < max_events - 1; j++)    
            {
                if (events[j].fd == fd)
                {
                    events[j].fd = 0;
                    events[j].events = 0;
                    break;
                }
            }
            client_shutdown(fd);
        }
    }
    else
    for (unsigned int i=0; i < max_events - 1; i++)    
    {
        if (events[i].fd == fd)
        {
            events[i].events |= (short) POLLIN;
            break;
        }
    }
}


void connectLoop(void)
{
    int nfds = 0;
    short mask;
    memset(events, 0 , sizeof(events));
    events[0].fd = sockfd;
    events[0].events = (POLLIN | POLLHUP | POLLERR);
    for (;;)
    {
        if (sockfd == -1) break;
        nfds = poll(events, max_events - 1, 500);
        //printf("nfds = %d\n", nfds);
        if (nfds ==  0) continue; // timeout
        if (nfds == -1) break; // error
        for (unsigned int i = 0; i < max_events - 1; i++)
        {
            if (nfds <= 0) break;
            if(events[i].revents == 0) continue;
            nfds--;
            if ((events[i].revents & (POLLHUP | POLLERR)) != 0)
            {
                do_full_shutdown();
                connection_shutdown();
                events[i].revents = 0;
                break;
            }
            else if ((events[i].revents & POLLIN) == POLLIN)
            {
                connection_read_avail();
                mask = POLLIN;
                events[i].events &= (~mask);
                continue;
            }
            else if ((events[i].revents & POLLOUT) == POLLOUT)
            {
                if (waitingforconn)
                {
                    waitingforconn = false;
                    connected_to_server();
                }
                else
                {
                    connection_write_avail();
                }
                mask = POLLOUT;
                events[i].events &= (~mask);
                continue;
            }
        }
    }
}

private:

const unsigned int max_events = 64;

};

} // namespace feal



#endif // _FEAL_BASESTREAM_POLL_H
