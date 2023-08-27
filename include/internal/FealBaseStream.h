//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _FEAL_BASESTREAM_H
#define _FEAL_BASESTREAM_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined (_WIN32)
#include <atomic>
#elif defined (__linux__)
#include <sys/epoll.h>
#else
#include <unistd.h>
#include <sys/event.h>
#include <cstring>
#endif


namespace feal
{

class BaseStream : public Tool
{
public:
BaseStream() = default;
BaseStream( const BaseStream & ) = default;
BaseStream& operator= ( const BaseStream & ) = default;
~BaseStream() = default;

protected:

handle_t sockfd = FEAL_INVALID_HANDLE;
bool waitingforconn = false;

virtual int  accept_new_conn(void);
virtual void client_read_avail(handle_t client_sockfd);
virtual void client_write_avail(handle_t client_sockfd);
virtual void client_shutdown(handle_t client_sockfd);
virtual void server_shutdown(void);
virtual void connected_to_server(handle_t fd);
virtual void connection_read_avail(void);
virtual void connection_write_avail(void);
virtual void connection_shutdown(void);

void serverLoop(void);

int  do_client_read_start(handle_t client_sockfd);
int  do_client_shutdown(handle_t client_sockfd);
int  do_full_shutdown(void);
void do_connect_in_progress(void);
void do_connect_ok(void);
void do_send_avail_notify(handle_t fd);

void connectLoop(void);

private:

#if defined (_WIN32)
#define FEALBASESTREAM_MAXEVENTS       (FD_SETSIZE > 64 ? 64 : FD_SETSIZE)
const int max_events = FEALBASESTREAM_MAXEVENTS;
handle_t sockread[FEALBASESTREAM_MAXEVENTS];
handle_t sockwrite[FEALBASESTREAM_MAXEVENTS];
handle_t sockexcpt[FEALBASESTREAM_MAXEVENTS];

#elif defined (__linux__)
const unsigned int max_events = 64;
int epfd = -1;
static int epoll_ctl_add(int epfd, handle_t fd, uint32_t events);
static int epoll_ctl_mod(int epfd, handle_t fd, uint32_t events);
#else

const unsigned int max_events = 64;
int kq = -1;
#endif


};


} // namespace feal

#endif // _FEAL_BASESTREAM_H
