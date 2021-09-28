#ifndef _FEAL_STREAM_POSIX_H
#define _FEAL_STREAM_POSIX_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{


class StreamGeneric : public BaseStream
{
public:
StreamGeneric() = default;
StreamGeneric( const StreamGeneric & ) = default;
StreamGeneric& operator= ( const StreamGeneric & ) = default;
~StreamGeneric() = default;


void shutdownTool(void);

errenum create_and_bind(feal::ipaddr* fa);
errenum create_and_connect(feal::ipaddr* fa);

#if defined(unix) || defined(__unix__) || defined(__unix)
errenum create_and_bind(struct sockaddr_un* su);
errenum create_and_connect(struct sockaddr_un* su);
errenum getpeername(struct sockaddr_un* su, socket_t fd = -1);
errenum getpeereid(uid_t* euid, gid_t* egid);
static errenum getpeereid(socket_t fd, uid_t* euid, gid_t* egid);
#endif

errenum listen(int backlog = 32);
errenum recv(void *buf, uint32_t len, int32_t* bytes, socket_t fd = -1);
errenum send(void *buf, uint32_t len, int32_t* bytes, socket_t fd = -1);
errenum disconnect_client(socket_t client_sockfd);
errenum disconnect_and_reset(void);
errenum getpeername(feal::ipaddr* fa, socket_t fd = -1);

protected:

std::thread serverThread;
std::thread connectThread;
std::map<int, std::weak_ptr<Actor>> mapReaders;

static void serverLoopLauncher(StreamGeneric *p);
static void connectLoopLauncher(StreamGeneric *p);

virtual void add_connect_events(void);
virtual void add_listen_events(void);
virtual void receiveEvent(std::shared_ptr<Event> pevt);

int accept_new_conn(void);
void client_read_avail(socket_t client_sockfd);
void client_write_avail(socket_t client_sockfd);
void client_shutdown(socket_t client_sockfd);
void server_shutdown(void);
void connected_to_server(socket_t fd);
void connection_read_avail(void);
void connection_write_avail(void);
void connection_shutdown(void);

};



template<typename Y>
class Stream : public StreamGeneric
{
public:
Stream() = default;
Stream( const Stream & ) = default;
Stream& operator= ( const Stream & ) = default;
~Stream() = default;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
}

template<class T>
errenum recv_start(T* p, socket_t client_sockfd)
{
    errenum res = FEAL_OK;
    std::weak_ptr<Actor> wkact;
    if (p)
    {
        wkact = p->shared_from_this();
        EvtDataReadAvail edra;
        EvtDataWriteAvail edwa;
        EvtClientShutdown ecsd;
        p->addEvent(p, edra);
        p->addEvent(p, edwa);
        p->addEvent(p, ecsd);
    }
    auto it = mapReaders.find(client_sockfd);
    if (it == mapReaders.end())
    {
        mapReaders[client_sockfd] = wkact;
        setnonblocking(client_sockfd);
        if (do_client_read_start(client_sockfd) == -1)
            res = static_cast<errenum>(errno);
    }
    return res;
}

protected:

void add_connect_events(void)
{
    if (actorptr == nullptr) return;
    EvtConnectedToServer ects;
    EvtDataReadAvail edra;
    EvtDataWriteAvail edwa;
    EvtConnectionShutdown ecs;
    actorptr->addEvent(actorptr, ects);
    actorptr->addEvent(actorptr, edra);
    actorptr->addEvent(actorptr, edwa);
    actorptr->addEvent(actorptr, ecs);
}

void add_listen_events(void)
{
    if (actorptr == nullptr) return;
    EvtIncomingConn eic;
    EvtServerShutdown ess;
    actorptr->addEvent(actorptr, eic);
    actorptr->addEvent(actorptr, ess);
}

void receiveEvent(std::shared_ptr<Event>& pevt)
{
    if (actorptr) actorptr->receiveEvent(pevt);
}

private:
Y* actorptr = nullptr;


};

} // namespace feal


#endif // _FEAL_STREAM_POSIX_H
