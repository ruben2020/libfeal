//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _FEAL_COMMON_WIN_H
#define _FEAL_COMMON_WIN_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <winsock2.h>
//#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>

#define SOCK_STARTUP() \
    WSADATA wsa; \
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) return 1;

#define SOCK_CLEANUP()  WSACleanup()

namespace feal
{

typedef enum
{
    FEAL_OK = 0,
    FEAL_E2BIG = E2BIG,
    FEAL_EACCES = WSAEACCES,
    FEAL_EADDRINUSE = WSAEADDRINUSE,
    FEAL_EADDRNOTAVAIL = WSAEADDRNOTAVAIL,
    FEAL_EAFNOSUPPORT = WSAEAFNOSUPPORT,
    FEAL_EAGAIN = EAGAIN,
    FEAL_EALREADY = WSAEALREADY,
    //FEAL_EBUSY = WSAEBUSY,
    FEAL_ECANCELED = WSAECANCELLED,
    //FEAL_ECHILD = WSAECHILD,
    FEAL_ECONNABORTED = WSAECONNABORTED,
    FEAL_ECONNREFUSED = WSAECONNREFUSED,
    FEAL_ECONNRESET = WSAECONNRESET,
    FEAL_EDEADLK = EDEADLK,
    FEAL_EDESTADDRREQ = WSAEDESTADDRREQ,
    FEAL_EDOM = EDOM,
    FEAL_EDQUOT = WSAEDQUOT,
    FEAL_EEXIST = EEXIST,
    FEAL_EFAULT = WSAEFAULT,
    FEAL_EFBIG = EFBIG,
    FEAL_EHOSTDOWN = WSAEHOSTDOWN,
    FEAL_EHOSTUNREACH = WSAEHOSTUNREACH,
    FEAL_EIDRM = EIDRM,
    FEAL_EILSEQ = EILSEQ,
    FEAL_EINPROGRESS = WSAEINPROGRESS,
    FEAL_EINTR = WSAEINTR,
    FEAL_EINVAL = WSAEINVAL,
    //FEAL_EIO = WSAEIO,
    FEAL_EISCONN = WSAEISCONN,
    //FEAL_EISDIR = WSAEISDIR,
    FEAL_ELOOP = WSAELOOP,
    FEAL_EMFILE = WSAEMFILE,
    FEAL_EMLINK = EMLINK,
    FEAL_EMSGSIZE = WSAEMSGSIZE,
    //FEAL_EMULTIHOP = WSAEMULTIHOP,
    FEAL_ENAMETOOLONG = WSAENAMETOOLONG,
    FEAL_ENETDOWN = WSAENETDOWN,
    FEAL_ENETRESET = WSAENETRESET,
    FEAL_ENETUNREACH = WSAENETUNREACH,
    //FEAL_ENFILE = WSAENFILE,
    FEAL_ENOBUFS = WSAENOBUFS,
    //FEAL_ENODATA = WSAENODATA,
    FEAL_ENODEV = ENODEV,
    //FEAL_ENOENT = WSAENOENT,
    FEAL_ENOEXEC = ENOEXEC,
    //FEAL_ENOLCK = WSAENOLCK,
    FEAL_ENOLINK = ENOLINK,
    FEAL_ENOMEM = WSA_NOT_ENOUGH_MEMORY,
    //FEAL_ENOMSG = WSAENOMSG,
    FEAL_ENOPROTOOPT = WSAENOPROTOOPT,
    //FEAL_ENOSPC = WSAENOSPC,
    //FEAL_ENOSR = WSAENOSR,
    //FEAL_ENOSTR = WSAENOSTR,
    //FEAL_ENOSYS = WSAENOSYS,
    //FEAL_ENOTBLK = WSAENOTBLK,
    FEAL_ENOTCONN = WSAENOTCONN,
    FEAL_ENOTDIR = ENOTDIR,
    FEAL_ENOTEMPTY = WSAENOTEMPTY,
    FEAL_ENOTRECOVERABLE = ENOTRECOVERABLE,
    FEAL_ENOTSOCK = WSAENOTSOCK,
    FEAL_ENOTSUP = ENOTSUP,
    FEAL_ENOTTY = ENOTTY,
    FEAL_ENXIO = ENXIO,
    FEAL_EOPNOTSUPP = WSAEOPNOTSUPP,
    FEAL_EOVERFLOW = EOVERFLOW,
    FEAL_EOWNERDEAD = EOWNERDEAD,
    //FEAL_EPERM = WSAEPERM,
    FEAL_EPFNOSUPPORT = WSAEPFNOSUPPORT,
    FEAL_EPIPE = EPIPE,
    FEAL_EPROTO = EPROTO,
    FEAL_EPROTONOSUPPORT = WSAEPROTONOSUPPORT,
    FEAL_EPROTOTYPE = WSAEPROTOTYPE,
    //FEAL_ERANGE = WSAERANGE,
    FEAL_EREMOTE = WSAEREMOTE,
    FEAL_EROFS = EROFS,
    FEAL_ESHUTDOWN = WSAESHUTDOWN,
    //FEAL_ESPIPE = WSAESPIPE,
    FEAL_ESOCKTNOSUPPORT = WSAESOCKTNOSUPPORT,
    FEAL_ESRCH = ESRCH,
    FEAL_ESTALE = WSAESTALE,
    //FEAL_ETIME = WSAETIME,
    FEAL_ETIMEDOUT = WSAETIMEDOUT,
    FEAL_ETOOMANYREFS = WSAETOOMANYREFS,
    FEAL_ETXTBSY = ETXTBSY,
    FEAL_EUSERS = WSAEUSERS,
    FEAL_EWOULDBLOCK = WSAEWOULDBLOCK,
    FEAL_EXDEV = EXDEV
} errenum;


typedef SOCKET handle_t;
#define FEAL_INVALID_HANDLE    (INVALID_SOCKET)
#define FEAL_HANDLE_ERROR      (SOCKET_ERROR)
#define FEAL_GETHANDLEERRNO    WSAGetLastError()
#define FEAL_SHUT_RDWR         SD_BOTH

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT   (0)
#endif

#ifndef MSG_CONFIRM
#define MSG_CONFIRM    (0)
#endif

typedef union sockaddr_ip {
    struct sockaddr sa;
    struct sockaddr_in in;
    struct sockaddr_in6 in6;
} sockaddr_ip;


typedef uint32_t EventId_t;
typedef uint32_t ActorId_t;
typedef enum {INET=AF_INET, INET6=AF_INET6} family_t;

typedef struct {
    enum {INET=AF_INET, INET6=AF_INET6} family;
    uint16_t port;
    char addr[INET6_ADDRSTRLEN];
} ipaddr;

typedef unsigned int flags_t;

int inet_pton(int af, const char *src, void *dst);
const char * inet_ntop(int af, const void *src, char *dst, socklen_t size);
void ipaddr_posix2feal(sockaddr_ip* su, ipaddr* fa);
int  ipaddr_feal2posix(ipaddr* fa, sockaddr_ip* su);
int  set_nonblocking(handle_t fd);
int  set_ipv6only(handle_t fd);
int  set_reuseaddr(handle_t fd, bool enable);
int  datareadavaillen(handle_t fd);

}


#endif // _FEAL_COMMON_WIN_H
