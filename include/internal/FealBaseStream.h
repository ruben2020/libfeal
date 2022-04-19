/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Dual-licensed under the Apache License, Version 2.0, and
 * the GNU General Public License, Version 2.0;
 * you may not use this file except in compliance
 * with either one of these licenses.
 *
 * You can find copies of these licenses in the included
 * LICENSE-APACHE2 and LICENSE-GPL2 files.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under these licenses is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the license texts for the specific language governing permissions
 * and limitations under the licenses.
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
 *
 */
 
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

socket_t sockfd = FEAL_INVALID_SOCKET;
bool waitingforconn = false;

virtual int  accept_new_conn(void);
virtual void client_read_avail(socket_t client_sockfd);
virtual void client_write_avail(socket_t client_sockfd);
virtual void client_shutdown(socket_t client_sockfd);
virtual void server_shutdown(void);
virtual void connected_to_server(socket_t fd);
virtual void connection_read_avail(void);
virtual void connection_write_avail(void);
virtual void connection_shutdown(void);

void serverLoop(void);

int  do_client_read_start(socket_t client_sockfd);
int  do_client_shutdown(socket_t client_sockfd);
int  do_full_shutdown(void);
void do_connect_in_progress(void);
void do_connect_ok(void);
void do_send_avail_notify(socket_t fd);

void connectLoop(void);

private:

#if defined (_WIN32)
#define FEALBASESTREAM_MAXEVENTS       (FD_SETSIZE > 64 ? 64 : FD_SETSIZE)
const int max_events = FEALBASESTREAM_MAXEVENTS;
socket_t sockread[FEALBASESTREAM_MAXEVENTS];
socket_t sockwrite[FEALBASESTREAM_MAXEVENTS];
socket_t sockexcpt[FEALBASESTREAM_MAXEVENTS];

#elif defined (__linux__)
const unsigned int max_events = 64;
int epfd = -1;
static int epoll_ctl_add(int epfd, socket_t fd, uint32_t events);
static int epoll_ctl_mod(int epfd, socket_t fd, uint32_t events);
#else

const unsigned int max_events = 64;
int kq = -1;
#endif


};


} // namespace feal

#endif // _FEAL_BASESTREAM_H
