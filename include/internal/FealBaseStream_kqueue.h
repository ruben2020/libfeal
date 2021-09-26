#ifndef _FEAL_BASESTREAM_KQUEUE_H
#define _FEAL_BASESTREAM_KQUEUE_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <unistd.h>
#include <sys/event.h>
#include <cstring>

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
int kq = -1;
bool waitingforconn = false;

virtual int  accept_new_conn(void){return -1;}
virtual void client_read_avail(socket_t client_sockfd){(void)(client_sockfd);}
virtual void client_write_avail(socket_t client_sockfd){(void)(client_sockfd);}
virtual void client_shutdown(socket_t client_sockfd){(void)(client_sockfd);}
virtual void server_shutdown(void){}
virtual void connected_to_server(socket_t fd){(void)(fd);}
virtual void connection_read_avail(void){}
virtual void connection_write_avail(void){}
virtual void connection_shutdown(void){}

void serverLoop(void)
{
    int nevts = 0;
    struct timespec tims;
    struct kevent change_event[4], event[max_events];
    memset(&change_event, 0, sizeof(change_event));
    memset(&event, 0, sizeof(event));
    tims.tv_sec = 0;
    tims.tv_nsec = 500000000; // 500ms
    kq = kqueue();
    EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent error");
        return;
    }
    for (;;)
    {
        nevts = kevent(kq, nullptr, 0, event, max_events - 1, (const struct timespec *) &tims);
        if (nevts == 0) continue;
        if (nevts == -1) break;
        for (int i = 0; i < nevts; i++)
        {
            if (sockfd == (int) event[i].ident)
            {
                if (((event[i].flags & (EV_EOF | EV_ERROR)) != 0)||
                    (accept_new_conn() == -1))
                {
                    do_full_shutdown();
                    server_shutdown();
                    break;
                }
                continue;
            }
            else if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                do_client_shutdown((int) event[i].ident);
                client_shutdown((int) event[i].ident);
                continue;
            }
            else if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                client_read_avail((int) event[i].ident);
                continue;
            }
            else if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                client_write_avail((int) event[i].ident);
                EV_SET(change_event, event[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
                kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
                continue;
            }
        }
    }
}

int do_client_read_start(socket_t client_sockfd)
{
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, client_sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    return kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
}

int do_client_shutdown(socket_t client_sockfd)
{
    int rc = shutdown(client_sockfd, SHUT_RDWR);
    close(client_sockfd);
    return rc;
}

int do_full_shutdown(void)
{
    int res = 0;
    if (sockfd != -1) res = shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    close(kq);
    kq = -1;
    sockfd = -1;
    waitingforconn = false;
    return res;
}

void do_connect_in_progress(void)
{
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    waitingforconn = true;
    kq = kqueue();
    EV_SET(change_event, sockfd, EVFILT_READ,  EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
    EV_SET(change_event, sockfd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
}

void do_connect_ok(void)
{
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    waitingforconn = false;
    kq = kqueue();
    EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        printf("do_connect_ok err %d\n", errno);
    }
    else connected_to_server(sockfd);
}

void do_send_avail_notify(int fd)
{
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
}

void connectLoop(void)
{
    int nevts = 0;
    struct timespec tims;
    struct kevent change_event[4], event[max_events];
    memset(&change_event, 0, sizeof(change_event));
    memset(&event, 0, sizeof(event));
    tims.tv_sec = 0;
    tims.tv_nsec = 500000000; // 500ms
    for (;;)
    {
        nevts = kevent(kq, nullptr, 0, event, max_events - 1, (const struct timespec *) &tims);
        if (nevts == 0) continue;
        if (nevts == -1) break;
        for (int i = 0; i < nevts; i++)
        {
            if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                do_full_shutdown();
                connection_shutdown();
                break;
            }
            else if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                connection_read_avail();
                continue;
            }
            else if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                if (waitingforconn)
                {
                    waitingforconn = false;
                    connected_to_server((int) event[i].ident);
                }
                else
                {
                    connection_write_avail();
                }
                EV_SET(change_event, sockfd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
                kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
                continue;
            }
        }
    }
}

private:

const unsigned int max_events = 64;

};



} // namespace feal



#endif // _FEAL_BASESTREAM_KQUEUE_H
