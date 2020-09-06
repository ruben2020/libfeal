#ifndef _FEAL_TCP_STREAM_POSIX_H
#define _FEAL_TCP_STREAM_POSIX_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

namespace feal
{

class TCPStream : public Stream
{
public:
TCPStream() = default;
TCPStream( const TCPStream & ) = default;
TCPStream& operator= ( const TCPStream & ) = default;
~TCPStream() = default;

sockerrenum initSock();
sockerrenum bindSock(const char *ipaddr, uint16_t port);
sockerrenum bindSock(uint32_t ipaddr, uint16_t port);



protected:



private:

struct sockaddr_in addr;



};



} // namespace feal


#endif // _FEAL_TCP_STREAM_POSIX_H
