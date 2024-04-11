#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
#include "curl/curl.h"
#include "json/json.h"
#include <string>  
#include <fstream>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

static size_t WriteCallback(void* content, size_t size, size_t nmemb, void* pointer)
{
    ((std::string*)pointer)->append((char*)content, size * nmemb);
    return size * nmemb;
}

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

void downloader(const char* url, const char* file_name) {
    CURL* easyhandle = curl_easy_init();
    curl_easy_setopt(easyhandle, CURLOPT_URL, url);
    FILE* file = fopen(file_name, "wb");
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, file);
    curl_easy_perform(easyhandle);
    curl_easy_cleanup(easyhandle);
    fclose(file);
}

int main(int argc, char* argv[])
{
    CURL* req = curl_easy_init();
    CURLcode res;
    std::string httpData;
    std::stringstream httpStream;
    /* add your tags here !!*/
    std::string tags;

    std::cout << "input your tags here!";
    std::cin >> tags;

    if (req) {

        curl_easy_setopt(req, CURLOPT_URL, ("https://www.sakugabooru.com/post.json?tags=" + tags).c_str());
        curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(req, CURLOPT_WRITEDATA, &httpData);
        res = curl_easy_perform(req);
        curl_easy_cleanup(req);
    }
    Json::Value json_obj;
    Json::CharReaderBuilder jsonReader;
    std::string errs;

    httpStream << httpData;

    if (!Json::parseFromStream(jsonReader, httpStream, &json_obj, &errs))
    {
        std::cerr << "could not parse the json file" << std::endl;
    }

    string pathname = ".\\" + tags;
    _mkdir(pathname.c_str());

    for (int i = 0; i < json_obj.size(); i++) {

        downloader(json_obj[i]["file_url"].asCString(), (pathname + ".\\" + tags + json_obj[i]["md5"].asString() + +"." + json_obj[i]["file_ext"].asString()).c_str());
    }
}