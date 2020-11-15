#pragma once
#include"status.h"
#include<unordered_map>
extern std::unordered_map<std::string, std::string> Mime_map;
namespace zhtcp
{
    namespace server
 {
    class HttpResponse
    {
        public:
        HttpResponse(){}
        char * fileptr;
        response::HttpStatusCode mstate;
        std::string sstate;
        std::string mime;
        std::string filepath;
        request::HTTP_VERSION version_;
        std::unordered_map<std::string , std::string> mHeaders;
        void addHeader(std::string a,std::string b)
        {
            mHeaders.insert({a,b});
        }
        void create_response(char *);
    };
 }
} 