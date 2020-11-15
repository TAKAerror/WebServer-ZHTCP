#pragma once
#include"httpResponse.h"
#include"status.h"
#include<string>
#include<unordered_map>
#include"epoll.h"
namespace zhtcp 
{   
     
 namespace server
 {
 //std::unordered_map<std::string, zhtcp::server::request::HTTP_HEADER> header_map; 
   /*std::unordered_map<std::string, request::HTTP_HEADER> header_map = {
        {"HOST",                      request::Host},
        {"USER-AGENT",                request::User_Agent},
        {"CONNECTION",                request::Connection},
        {"ACCEPT-ENCODING",           request::Accept_Encoding},
        {"ACCEPT-LANGUAGE",           request::Accept_Language},
        {"ACCEPT",                    request::Accept},
        {"CACHE-CONTROL",             request::Cache_Control},
        {"UPGRADE-INSECURE-REQUESTS", request::Upgrade_Insecure_Requests}
};   */  


class do_requestArgs
      {
        public:
        int connfd;
        int state;
      };
 class HttpRequest
 {
     public:
       HttpRequest(){};
       void setVersion(request::HTTP_VERSION version){version_=version;};
       request::HTTP_VERSION version_;
       request::HTTP_METHOD  method_;
       request::HTTP_HEADER header_;
       std::string uri_; 
       std::unordered_map<request::HTTP_HEADER, std::string> mHeaders;

        
 };
 status::LINE_STATUS parse_line(char * buffer,int & checked_index,int & read_index);
 status::HTTP_CODE parse_requestline(char * temp,status::CHECK_STATE &checkstate,HttpRequest& request);
 status::HTTP_CODE parse_headers(char * temp,HttpRequest &request);
 status::HTTP_CODE parse_content(char *buff,int & checked_index,status::CHECK_STATE &checkstate,int &read_index,int & start_line,HttpRequest &request);
 
    }
}