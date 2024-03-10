//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include "feal.h"

int feal::inet_pton(int af, const char *src, void *dst)
{
  struct sockaddr_storage ss;
  int size = sizeof(ss);
  char src_copy[INET6_ADDRSTRLEN+1];

  ZeroMemory(&ss, sizeof(ss));
  
  strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
  src_copy[INET6_ADDRSTRLEN] = 0;

  if (WSAStringToAddressA(src_copy, af, nullptr, (struct sockaddr *)&ss, &size) == 0) {
    switch(af) {
      case AF_INET:
    *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
    return 10;
      case AF_INET6:
    *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
    return 10;
    }
  }
  return SOCKET_ERROR;
}

const char *feal::inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
  struct sockaddr_storage ss;
  unsigned long s = size;

  ZeroMemory(&ss, sizeof(ss));
  ss.ss_family = af;

  switch(af) {
    case AF_INET:
      ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
      break;
    case AF_INET6:
      ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
      break;
    default:
      return nullptr;
  }
  /* cannot directly use &size because of strict aliasing rules */
  return (WSAAddressToStringA((struct sockaddr *)&ss, sizeof(ss), nullptr, dst, &s) == 0)?
          dst : nullptr;
}

void feal::ipaddr_posix2feal(sockaddr_ip* su, feal::ipaddr* fa)
{
    if ((fa == nullptr) || (su == nullptr)) return;
    fa->family = su->sa.sa_family == AF_INET6 ? feal::ipaddr::INET6 : feal::ipaddr::INET;
    if (fa->family == feal::ipaddr::INET)
    {
        fa->port = ntohs(su->in.sin_port);
        feal::inet_ntop(AF_INET, &(su->in.sin_addr), fa->addr, INET_ADDRSTRLEN);
    }
    else if (fa->family == feal::ipaddr::INET6)
    {
        fa->port = ntohs(su->in6.sin6_port);
        feal::inet_ntop(AF_INET6, &(su->in6.sin6_addr), fa->addr, INET6_ADDRSTRLEN);
    }
}

int feal::ipaddr_feal2posix(feal::ipaddr* fa, sockaddr_ip* su)
{
    int res = SOCKET_ERROR;
    if ((fa == nullptr) || (su == nullptr)) return SOCKET_ERROR;
    su->sa.sa_family = (unsigned short) fa->family;
    if (fa->family == feal::ipaddr::INET)
    {
        su->in.sin_port = htons(fa->port);
        res = feal::inet_pton(AF_INET, fa->addr, &(su->in.sin_addr));
    }
    else if (fa->family == feal::ipaddr::INET6)
    {
        su->in6.sin6_port = htons(fa->port);
        res = feal::inet_pton(AF_INET6, fa->addr, &(su->in6.sin6_addr));
    }
    return res;
}

int feal::setnonblocking(handle_t fd)
{
    ULONG NonBlock = 1;
    if (ioctlsocket(fd, FIONBIO, &NonBlock) == SOCKET_ERROR)
    {
        return SOCKET_ERROR;
    }
   return 0;
}

int feal::setipv6only(handle_t fd)
{
    int on = 1;
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
        (char *)&on, sizeof(on)) == -1)
    {
        return SOCKET_ERROR;
    }
    return 0;
}

int feal::datareadavaillen(handle_t fd)
{
    ULONG len;
    int ret = SOCKET_ERROR;
    if (ioctlsocket(fd, FIONREAD, &len) != -1)
        {
            ret = len;
        }
    return ret;
}
