//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "ActorA.h"

#include <cstdio>
#include <future>
#include <string>

#include "ActorsManager.h"

#define DOWNLOADURL "https://raw.githubusercontent.com/ruben2020/libfeal/refs/heads/master/NOTICE"
#if defined(_WIN32)
#define FILEDEST "C:\\Users\\AppData\\Local\\Temp\\NOTICE.txt"
#else
#define FILEDEST "/tmp/NOTICE.txt"
#endif

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");
    std::promise<std::shared_ptr<EvtCurlPromiseComplete>> curl_promise;
    fut_curl = curl_promise.get_future().share();
    printf("Trying to use libcurl to download\nfrom %s\nto %s\n\n", DOWNLOADURL, FILEDEST);
    curl_work_thread = std::thread(&curlThreadLauncher, this, std::string(DOWNLOADURL),
                                   std::string(FILEDEST), std::move(curl_promise));
    subscribePromise(this, fut_curl);
}

void ActorA::pauseActor(void)
{
    printf("ActorA pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA shutdownActor\n");
}

void ActorA::handleEvent(std::shared_ptr<EvtCurlPromiseComplete> pevt)
{
    if (!pevt)
        return;
    printf("ActorA EvtCurlPromiseComplete\n");
    printf("Results back from curl:\n Download %s (%lld bytes)\n",
           ((EvtCurlPromiseComplete *)pevt.get())->success ? "successful" : "not successful",
           (long long)((EvtCurlPromiseComplete *)pevt.get())->bytes);
    if (curl_work_thread.joinable())
        curl_work_thread.join();
    std::shared_ptr<feal::Event> pevt2 = std::make_shared<EvtWorkDone>();
    publishEvent(pevt2);
}

void ActorA::curlThreadLauncher(ActorA *ptr, std::string url, std::string filepath,
                                std::promise<std::shared_ptr<EvtCurlPromiseComplete>> prom)
{
    if (ptr)
        ptr->curl_loop(std::move(url), std::move(filepath), std::move(prom));
}

void ActorA::curl_loop(std::string url, std::string filepath,
                       std::promise<std::shared_ptr<EvtCurlPromiseComplete>> prom)
{
    // Based on https://curl.se/libcurl/c/url2file.html and
    // https://curl.se/libcurl/c/chkspeed.html
    CURL *curl_handle;
    FILE *pagefile;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    pagefile = fopen(filepath.c_str(), "wb");
    CURLcode res = CURLE_WRITE_ERROR;
    if (pagefile)
    {
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
        res = curl_easy_perform(curl_handle);  // blocking operation
        fclose(pagefile);
    }
    curl_off_t val = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_SIZE_DOWNLOAD_T, &val);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    std::shared_ptr<EvtCurlPromiseComplete> sp = std::make_shared<EvtCurlPromiseComplete>();
    sp.get()->bytes = val;
    sp.get()->success = (CURLE_OK == res);
    printf("Work completed!\n");
    prom.set_value(sp);
}
