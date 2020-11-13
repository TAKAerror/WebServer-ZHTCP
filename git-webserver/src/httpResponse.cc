#include<unordered_map>
#include"../include/httpResponse.h"
#include<iterator>
std::unordered_map<std::string,std::string> Mime_map = {
        {".html", "text/html"},
        {".xml", "text/xml"},
        {".xhtml", "application/xhtml+xml"},
        {".txt", "text/plain"},
        {".rtf", "application/rtf"},
        {".pdf", "application/pdf"},
        {".word", "application/msword"},
        {".png", "image/png"},
        {".gif", "image/gif"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".au", "audio/basic"},
        {".mpeg", "video/mpeg"},
        {".mpg", "video/mpeg"},
        {".avi", "video/x-msvideo"},
        {".gz", "application/x-gzip"},
        {".tar", "application/x-tar"},
        {".css", "text/css"},
        {"", "text/plain"},
        {"default","text/plain"}
};

void zhtcp::server::HttpResponse::create_response(char * head)
{
    if(version_==request::HTTP_10)
       sprintf(head,"HTTP/1.0 %d %s\r\n",mstate,sstate.c_str());
    else
    {
       sprintf(head,"HTTP/1.1 %d %s\r\n",mstate,sstate.c_str());
    }
    for(auto it=mHeaders.begin();it!=mHeaders.end();it++)
       sprintf(head,"%s%s: %s\r\n",head,it->first.c_str(),it->second.c_str());
    sprintf(head, "%sContent-Type: %s\r\n", head, mime.c_str());
     if(version_==request::HTTP_11)//暂时不支持长连接
      sprintf(head, "%sConnection: close\r\n", head);

    
}