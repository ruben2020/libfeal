//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <curl/curl.h>
#include "ActorA.h"


#define DOWNLOADURL "https://raw.githubusercontent.com/ruben2020/libfeal/refs/heads/master/NOTICE"

void ActorA::initActor(void)
{
    printf("ActorA::initActor\n");
    timers.init(this);
    dmon.init(this);
    dmon.subscribeReadAvail<EvtCurlMReadAvail>();
    dmon.subscribeWriteAvail<EvtCurlMWriteAvail>();
    dmon.subscribeDescErr<EvtCurlMErr>();
}

void ActorA::startActor(void)
{
    int running_handles;
    printf("ActorA::startActor\n");
    curl_global_init(CURL_GLOBAL_ALL);
    multi = curl_multi_init();
    curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, cb_socket);
    curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, this);
    curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, cb_timeout);
    curl_multi_setopt(multi, CURLMOPT_TIMERDATA, this);
    add_download(DOWNLOADURL, 1);
    curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0, &running_handles);
}

void ActorA::pauseActor(void)
{
    printf("ActorA::pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("ActorA shutdown complete\n");
}

void ActorA::handleEvent(std::shared_ptr<EvtCurlTimer> pevt)
{
    int running_handles;
    if (!pevt) return;
    printf("ActorA::EvtCurlTimer\n");
    curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                &running_handles);
    check_multi_info();
}

void ActorA::handleEvent(std::shared_ptr<EvtCurlMReadAvail> pevt)
{
    int running_handles;
    int flags = 0;
    if (!pevt) return;
    printf("ActorA::EvtCurlMReadAvail\n");
    flags |= CURL_CSELECT_IN;
    curl_multi_socket_action(multi, sockfd, flags,
                           &running_handles);
    check_multi_info();
}

void ActorA::handleEvent(std::shared_ptr<EvtCurlMWriteAvail> pevt)
{
    int running_handles;
    int flags = 0;
    if (!pevt) return;
    printf("ActorA::EvtCurlMWriteAvail\n");
    flags |= CURL_CSELECT_OUT;
    curl_multi_socket_action(multi, sockfd, flags,
                           &running_handles);
    check_multi_info();
}

void ActorA::handleEvent(std::shared_ptr<EvtCurlMErr> pevt)
{
    int running_handles;
    int flags = 0;
    if (!pevt) return;
    printf("ActorA::EvtCurlMErr\n");
    flags |= CURL_CSELECT_ERR;
    curl_multi_socket_action(multi, sockfd, flags,
                           &running_handles);
    check_multi_info();
}

void ActorA::add_download(const char *url, int num)
{
    char filename[50];
    FILE *file;
    CURL *handle;
 
    printf("ActorA::add_download\n");
    snprintf(filename, 50, "/tmp/%d.download", num);
 
    file = fopen(filename, "wb");
    if(!file) {
      fprintf(stderr, "Error opening %s\n", filename);
      return;
    }
    fpdownload = file;
 
    handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(handle, CURLOPT_PRIVATE, file);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_multi_add_handle(multi, handle);
    fprintf(stderr, "Added download %s -> %s\n", url, filename);
}

int ActorA::cb_socket(CURL *easy, curl_socket_t s, int action,
                     ActorA *act, void *socketp)
{
    struct curl_context *curl_context;
    (void)easy;
 
    printf("ActorA::cb_socket\n");
    switch(action) 
    {
    case CURL_POLL_IN:
    case CURL_POLL_OUT:
    case CURL_POLL_INOUT:
        printf("CURL_POLL_INOUT\n");
        if (act->active_state == false) return 1;
        curl_context = socketp ?
          (struct curl_context *) socketp : act->create_curl_context(s, act);
        curl_multi_assign(act->multi, s, (void *) curl_context);
        act->sockfd = curl_context->sockfd;
        act->dmon.monitor(curl_context->sockfd);
        break;
    case CURL_POLL_REMOVE:
        printf("CURL_POLL_REMOVE\n");
        act->active_state = false;
        act->timers.stopTimer<EvtCurlTimer>();
        curl_multi_assign(act->multi, s, NULL);
        if (act->multi) curl_multi_cleanup(act->multi);
        act->dmon.close_and_reset();
        if (act->fpdownload) fclose(act->fpdownload);
        //act->shutdown();
        break;
    default:
        printf("CURL_POLL DEFAULT\n");    
        //act->shutdown();
    }
 
  return 0;
}

int ActorA::cb_timeout(CURLM *multi, long timeout_ms, ActorA *act)
{
    (void)multi;
    printf("ActorA::cb_timeout\n");
    if(timeout_ms < 0)
        act->timers.stopTimer<EvtCurlTimer>();
    else 
    {
        if(timeout_ms == 0) timeout_ms = 1;
        //timeout_ms *= 10;
        printf("set timeout to %ld ms\n", timeout_ms);
        act->timers.startTimer<EvtCurlTimer>(std::chrono::milliseconds(timeout_ms));
    }
    return 0;
}

void ActorA::check_multi_info(void)
{
    char *done_url;
    CURLMsg *message;
    int pending;
    CURL *easy_handle;
    FILE *file;
 
    printf("ActorA::check_multi_info\n");
    while((message = curl_multi_info_read(multi, &pending))) 
    {
        switch(message->msg)
        {
            case CURLMSG_DONE:
                printf("CURLMSG_DONE\n");
                easy_handle = message->easy_handle;
                curl_easy_getinfo(easy_handle, CURLINFO_EFFECTIVE_URL, &done_url);
                curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &file);
                printf("%s DONE\n", done_url);
 
                curl_multi_remove_handle(multi, easy_handle);
                curl_easy_cleanup(easy_handle);
                if(file) 
                {
                    fclose(file);
                }
                break;
 
            default:
                fprintf(stderr, "CURLMSG default\n");
                break;
        }
  }
}

struct curl_context *ActorA::create_curl_context(curl_socket_t sockfd,
                                                ActorA *act)
{
  struct curl_context *context;
  printf("ActorA::create_curl_context\n");
  context = (struct curl_context *) malloc(sizeof(*context));
  context->sockfd = sockfd;
  context->act = act; 
  return context;
}
