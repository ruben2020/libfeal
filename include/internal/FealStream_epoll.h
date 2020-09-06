#ifndef _FEAL_STREAM_EPOLL_H
#define _FEAL_STREAM_EPOLL_H

namespace feal
{

class Stream
{
public:
Stream() = default;
Stream( const Stream & ) = default;
Stream& operator= ( const Stream & ) = default;
~Stream() = default;

enum sockerrenum
{
    S_OK = 0,
    S_EACCES = EACCES,
    S_EAFNOSUPPORT = EAFNOSUPPORT,
    S_EINVAL = EINVAL,
    S_EMFILE = EMFILE,
    S_ENFILE = ENFILE,
    S_ENOBUFS = ENOBUFS,
    S_ENOMEM = ENOMEM,
    S_EPROTONOSUPPORT = EPROTONOSUPPORT,
    S_EADDRINUSE = EADDRINUSE,
    S_EBADF = EBADF,
    S_ENOTSOCK = ENOTSOCK,
    S_EADDRNOTAVAIL = EADDRNOTAVAIL,
    S_EFAULT = EFAULT,
    S_ELOOP = ELOOP,
    S_ENAMETOOLONG = ENAMETOOLONG,
    S_ENOENT = ENOENT,
    S_ENOTDIR = ENOTDIR,
    S_EROFS = EROFS,
    S_ENOTSOCK = ENOTSOCK,
    S_EOPNOTSUPP = EOPNOTSUPP
};

template< typename T, typename Y >
sockerrenum listenSock(Y* p, int backlog)
{
    sockerrenum res = S_OK;
    if (listen(sockfd, backlog) == -1)
            res = static_cast<sockerrenum>(errno);
    else
    {
        
    }
    return res;
}

protected:

int sockfd;


private:


};



} // namespace feal



#endif // _FEAL_STREAM_EPOLL_H
