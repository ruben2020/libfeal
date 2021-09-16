#ifndef _FEAL_COMMON_H
#define _FEAL_COMMON_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#if defined(unix) || defined(__unix__) || defined(__unix)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace feal
{
    
typedef enum
{
    S_OK = 0,
    S_E2BIG = E2BIG,
    S_EACCES = EACCES,
    S_EADDRINUSE = EADDRINUSE,
    S_EADDRNOTAVAIL = EADDRNOTAVAIL,
    S_EAFNOSUPPORT = EAFNOSUPPORT,
    S_EAGAIN = EAGAIN,
    S_EALREADY = EALREADY,
    S_EBUSY = EBUSY,
    S_ECANCELED = ECANCELED,
    S_ECHILD = ECHILD,
    S_ECONNABORTED = ECONNABORTED,
    S_ECONNREFUSED = ECONNREFUSED,
    S_ECONNRESET = ECONNRESET,
    S_EDEADLK = EDEADLK,
    S_EDESTADDRREQ = EDESTADDRREQ,
    S_EDOM = EDOM,
    S_EDQUOT = EDQUOT,
    S_EEXIST = EEXIST,
    S_EFAULT = EFAULT,
    S_EFBIG = EFBIG,
    S_EHOSTDOWN = EHOSTDOWN,
    S_EHOSTUNREACH = EHOSTUNREACH,
    S_EIDRM = EIDRM,
    S_EILSEQ = EILSEQ,
    S_EINPROGRESS = EINPROGRESS,
    S_EINTR = EINTR,
    S_EINVAL = EINVAL,
    S_EIO = EIO,
    S_EISCONN = EISCONN,
    S_EISDIR = EISDIR,
    S_ELOOP = ELOOP,
    S_EMFILE = EMFILE,
    S_EMLINK = EMLINK,
    S_EMSGSIZE = EMSGSIZE,
    S_EMULTIHOP = EMULTIHOP,
    S_ENAMETOOLONG = ENAMETOOLONG,
    S_ENETDOWN = ENETDOWN,
    S_ENETRESET = ENETRESET,
    S_ENETUNREACH = ENETUNREACH,
    S_ENFILE = ENFILE,
    S_ENOBUFS = ENOBUFS,
    S_ENODATA = ENODATA,
    S_ENODEV = ENODEV,
    S_ENOENT = ENOENT,
    S_ENOEXEC = ENOEXEC,
    S_ENOLCK = ENOLCK,
    S_ENOLINK = ENOLINK,
    S_ENOMEM = ENOMEM,
    S_ENOMSG = ENOMSG,
    S_ENOPROTOOPT = ENOPROTOOPT,
    S_ENOSPC = ENOSPC,
    S_ENOSR = ENOSR,
    S_ENOSTR = ENOSTR,
    S_ENOSYS = ENOSYS,
    S_ENOTBLK = ENOTBLK,
    S_ENOTCONN = ENOTCONN,
    S_ENOTDIR = ENOTDIR,
    S_ENOTEMPTY = ENOTEMPTY,
    S_ENOTRECOVERABLE = ENOTRECOVERABLE,
    S_ENOTSOCK = ENOTSOCK,
    S_ENOTSUP = ENOTSUP,
    S_ENOTTY = ENOTTY,
    S_ENXIO = ENXIO,
    S_EOPNOTSUPP = EOPNOTSUPP,
    S_EOVERFLOW = EOVERFLOW,
    S_EOWNERDEAD = EOWNERDEAD,
    S_EPERM = EPERM,
    S_EPFNOSUPPORT = EPFNOSUPPORT,
    S_EPIPE = EPIPE,
    S_EPROTO = EPROTO,
    S_EPROTONOSUPPORT = EPROTONOSUPPORT,
    S_EPROTOTYPE = EPROTOTYPE,
    S_ERANGE = ERANGE,
    S_EREMOTE = EREMOTE,
    S_EROFS = EROFS,
    S_ESHUTDOWN = ESHUTDOWN,
    S_ESPIPE = ESPIPE,
    S_ESOCKTNOSUPPORT = ESOCKTNOSUPPORT,
    S_ESRCH = ESRCH,
    S_ESTALE = ESTALE,
    S_ETIME = ETIME,
    S_ETIMEDOUT = ETIMEDOUT,
    S_ETOOMANYREFS = ETOOMANYREFS,
    S_ETXTBSY = ETXTBSY,
    S_EUSERS = EUSERS,
    S_EWOULDBLOCK = EWOULDBLOCK,
    S_EXDEV = EXDEV

#if defined (__linux__)
    ,
    S_EBADE = EBADE,
    S_EBADF = EBADF,
    S_EBADFD = EBADFD,
    S_EBADMSG = EBADMSG,
    S_EBADR = EBADR,
    S_EBADRQC = EBADRQC,
    S_EBADSLT = EBADSLT,
    S_ECHRNG = ECHRNG,
    S_ECOMM = ECOMM,
    S_EDEADLOCK = EDEADLOCK,
    S_EHWPOISON = EHWPOISON,
    S_EISNAM = EISNAM,
    S_EKEYEXPIRED = EKEYEXPIRED,
    S_EKEYREJECTED = EKEYREJECTED,
    S_EKEYREVOKED = EKEYREVOKED,
    S_EL2HLT = EL2HLT,
    S_EL2NSYNC = EL2NSYNC,
    S_EL3HLT = EL3HLT,
    S_EL3RST = EL3RST,
    S_ELIBACC = ELIBACC,
    S_ELIBBAD = ELIBBAD,
    S_ELIBMAX = ELIBMAX,
    S_ELIBSCN = ELIBSCN,
    S_ELIBEXEC = ELIBEXEC,
    S_ENOMEDIUM = ENOMEDIUM,
    S_ENOANO = ENOANO,
    S_ENOKEY = ENOKEY,
    S_EMEDIUMTYPE = EMEDIUMTYPE,
    S_ENONET = ENONET,
    S_ENOPKG = ENOPKG,
    S_ENOTUNIQ = ENOTUNIQ,
    S_EREMCHG = EREMCHG,
    S_EREMOTEIO = EREMOTEIO,
    S_ERESTART = ERESTART,
    S_ERFKILL = ERFKILL,
    S_ESTRPIPE = ESTRPIPE,
    S_EUCLEAN = EUCLEAN,
    S_EUNATCH = EUNATCH,
    S_EXFULL = EXFULL
#endif

} sockerrenum;


typedef int socket_t;

typedef union sockaddr_ip {
    struct sockaddr sa;
    struct sockaddr_in in;
    struct sockaddr_in6 in6;
} sockaddr_ip;

#endif

    typedef uint32_t EventId_t;
    typedef uint32_t ActorId_t;
    typedef enum {INET=AF_INET, INET6=AF_INET6, UNIX=AF_UNIX} family_t;

    typedef struct {
        enum {INET=AF_INET, INET6=AF_INET6} family;
        uint16_t port;
        char addr[INET6_ADDRSTRLEN];
    } ipaddr;

#if defined(unix) || defined(__unix__) || defined(__unix)
void ipaddr_posix2feal(sockaddr_ip* su, ipaddr* fa);
int  ipaddr_feal2posix(ipaddr* fa, sockaddr_ip* su);
int  setnonblocking(int fd);
int  setipv6only(int fd);
int  datareadavaillen(int fd);
#endif

}

#endif // _FEAL_COMMON_H
