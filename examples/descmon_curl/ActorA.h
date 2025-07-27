//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _ACTORA_H
#define _ACTORA_H

#include "feal.h"

class ActorA;

struct curl_context {
  curl_socket_t sockfd;
  ActorA *act;
};

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtCurlTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtCurlMReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtCurlMWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtCurlMErr, EventComm)


class ActorA : public feal::Actor
{

public:

ActorA() = default;
~ActorA() = default;

void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtCurlTimer> pevt);
void handleEvent(std::shared_ptr<EvtCurlMReadAvail> pevt);
void handleEvent(std::shared_ptr<EvtCurlMWriteAvail> pevt);
void handleEvent(std::shared_ptr<EvtCurlMErr> pevt);

static int cb_socket(CURL *easy, curl_socket_t s, int action,
                ActorA *act, void *socketp);
static int cb_timeout(CURLM *multi, long timeout_ms, ActorA *act);

protected:

feal::Timers<ActorA> timers;
feal::DescMon<ActorA> dmon;

private:
CURLM *multi = nullptr;
feal::handle_t sockfd;
FILE *fpdownload = nullptr;
std::atomic_bool active_state = true;

void add_download(const char *url, int num);
void check_multi_info(void);
struct curl_context *create_curl_context(curl_socket_t sockfd,
                                                ActorA *act);

};

#endif // _ACTORA_H

