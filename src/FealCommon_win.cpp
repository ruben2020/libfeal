//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
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

int feal::set_nonblocking(handle_t fd)
{
    ULONG NonBlock = 1;
    if (ioctlsocket(fd, FIONBIO, &NonBlock) == SOCKET_ERROR)
    {
        return SOCKET_ERROR;
    }
   return 0;
}

int feal::set_ipv6only(handle_t fd)
{
    int on = 1;
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
        (char *)&on, sizeof(on)) == -1)
    {
        return SOCKET_ERROR;
    }
    return 0;
}

int feal::set_reuseaddr(handle_t fd, bool enable)
{
    char opt = enable ? 1 : 0;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, 
            (const char *) &opt, (int) sizeof(opt));
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

std::string feal::get_addr(sockaddr_all* sa)
{
    char arr[200];
    if (sa == nullptr) return std::string();
    if (sa->sa.sa_family == AF_INET)
    {
        feal::inet_ntop(AF_INET, &(sa->sa), arr, 200);
    }
    else if (sa->sa.sa_family == AF_INET6)
    {
        feal::inet_ntop(AF_INET6, &(sa->sa), arr, 200);
    }
    else
    {
        arr[0] = 0;
    }
    return std::string(arr);
}

std::string feal::get_port(sockaddr_all* sa)
{
    char arr[10];
    if (sa == nullptr) return std::string();
    if (sa->sa.sa_family == AF_INET)
    {
        snprintf(arr, 10, "%d", ntohs(sa->in.sin_port));
    }
    else if (sa->sa.sa_family == AF_INET6)
    {
        snprintf(arr, 10, "%d", ntohs(sa->in6.sin6_port));
    }
    else
    {
        snprintf(arr, 10, "%d", 0);
    }
    return std::string(arr);
}

