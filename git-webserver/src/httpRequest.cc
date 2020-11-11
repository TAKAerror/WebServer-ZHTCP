 #include"../include/httpRequest.h"
 #include<assert.h>
 #include<string.h>
 #include<iostream>
 #include<unordered_map>
 #include<algorithm>
 #include<iostream>
 #include"utils.cpp"
 #include"../include/epoll.h"
 std::unordered_map<std::string, zhtcp::server::request::HTTP_HEADER> header_map = {
        {"HOST",                      zhtcp::server::request::Host},
        {"USER-AGENT",                zhtcp::server::request::User_Agent},
        {"CONNECTION",                zhtcp::server::request::Connection},
        {"ACCEPT-ENCODING",           zhtcp::server::request::Accept_Encoding},
        {"ACCEPT-LANGUAGE",           zhtcp::server::request::Accept_Language},
        {"ACCEPT",                    zhtcp::server::request::Accept},
        {"CACHE-CONTROL",             zhtcp::server::request::Cache_Control},
        {"UPGRADE-INSECURE-REQUESTS", zhtcp::server::request::Upgrade_Insecure_Requests}
};

 zhtcp::server::status::LINE_STATUS zhtcp::server::parse_line(char * buffer,int & checked_index,int & read_index)
 {
   char temp;
   for(;checked_index<read_index;checked_index++)
     {
       temp=buffer[checked_index];
       if(temp=='\r')
         {
           if(buffer[checked_index+1]=='\n')
             {
                 buffer[checked_index++]='\0';
                 buffer[checked_index++]='\0';
                 return status::LINE_OK;
             }
           else if(checked_index+1==read_index)
                 return status::LINE_MORE;
           else
           {
                return status::LINE_BAD;
           }
           
         }
     } 
     return status::LINE_MORE;
 };
 zhtcp::server::status::HTTP_CODE zhtcp::server::parse_requestline(char * temp,status::CHECK_STATE &checkstate,HttpRequest& request)
 {
  char *uri=strpbrk(temp," \t");   //第一个在str1的str2
    if(!uri)
     return status::BAD_REQUEST;
  *uri++='\0';
  char * method=temp;
  if(strcasecmp(method,"GET")==0)
     request.method_=request::GET;
  else
  {
        return status::BAD_REQUEST;
  }
  uri+=strspn(uri," \t");//strspn::第一个不在str1的str2
  if(!uri)
     return status::BAD_REQUEST;
  char * version=strpbrk(uri," \t");
  if(!version)
      return status::BAD_REQUEST;
  *version++='\0';
  if(strncasecmp(version,"HTTP/1.1",8)==0)
     request.version_=request::HTTP_10;
  else
  {
    std::cout<<"error http version"<<std::endl;
  }
  if(strncasecmp(uri,"http://",7)==0)
  {
      uri+=7;
      uri=strchr(uri,'/');
  }
  if(!uri ||uri[0]!='/')
     return status::BAD_REQUEST;
  std::cout<<"uri:"<<uri<<std::endl;
  request.uri_=std::string(uri);
  checkstate=status::CHECK_STATE::PARSE_HEADER;
 };
 zhtcp::server::status::HTTP_CODE zhtcp::server::parse_headers(char * temp,HttpRequest &request)
 {
   if(temp[0]=='\0');
     return status::GET_REQUEST;
   char *key=NULL;char * value=NULL;
   sscanf(temp, "%[^:]:%[^:]", key, value);
   std::string key_s(key);
   std::string value_s(value);
   std::transform(key_s.begin(), key_s.end(), key_s.begin(), ::toupper);//变大写
    if (key_s == std::string("UPGRADE-INSECURE-REQUESTS")) {
        return status::NO_REQUEST;    
    }
    decltype(header_map)::iterator it;
    if ((it = header_map.find(trim(key_s))) != (header_map.end())) {//根据“HOST”找header_map里面存的enum的“Host”
        request.mHeaders.insert(std::make_pair(it->second,trim(value_s)));//存进mHeaders。 std::unordered_map<HTTP_HEADER, std::string, EnumClassHash> mHeaders;
    } else 
    {
        std::cout << "Header no support: " << key << " : " << value << std::endl;
    }

    return status::NO_REQUEST;
 };
 zhtcp::server::status::HTTP_CODE zhtcp::server::parse_content(char *buffer,int & checked_index,status::CHECK_STATE &checkstate,int &read_index,int & start_line,HttpRequest &request)
 {
   std::cout<<"parse_content"<<std::endl;
   status::LINE_STATUS linestatus=status::LINE_OK;
   status::HTTP_CODE retcode=status::NO_REQUEST;
   while((linestatus=parse_line(buffer,checked_index,read_index))==status::LINE_OK)
   {
       char *temp=buffer+start_line;
       start_line+=checked_index;
      switch(checkstate)
        {case status::PARSE_REQUESTLINE:
        {
           std::cout<<"PARSE_REQUESTLINE"<<std::endl;
          retcode=parse_requestline(temp,checkstate,request);
          if (retcode==status::BAD_REQUEST)
              {
                std::cout<<"PARSE_REQUESTLINE BAD_REQUEST"<<std::endl;
                return status::BAD_REQUEST;
              }
          break;
        }
        case status::PARSE_HEADER:
        {
           std::cout<<"PARSE_HEADER"<<std::endl;
            retcode=parse_headers(temp,request);
            if (retcode==status::BAD_REQUEST)
              {std::cout<<"PARSE_HEADER BAD_REQUEST"<<std::endl;
              return status::BAD_REQUEST;
              
              }
            if(retcode==status::GET_REQUEST)
              {std::cout<<"PARSE_HEADER GET_REQUEST"<<std::endl;
              return status::GET_REQUEST;
              
              }
          break;
        }
        }
   }
   if( linestatus==status::LINE_MORE)
        {std::cout<<"LINE_MORE"<<std::endl;
       return status::NO_REQUEST;}
    else
    {
       std::cout<<"OUT parse_line BAD_REQUEST"<<std::endl;
        return status::BAD_REQUEST;
    }
    
   
 }
