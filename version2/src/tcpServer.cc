#include <cstring>
#include <cstdio>
#include"../include/tcpServer.h"
#include<iostream>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <sys/socket.h>
#include"../include/epoll.h"
#include"../include/httpRequest.h"
#include"../include/httpResponse.h"
#include<unordered_map>
#include<sys/epoll.h>
#include <sys/stat.h>
#include <sys/mman.h>
using std::placeholders::_1;
using std::placeholders::_2;
static const char * basepath="/home/hp-2/zhtcp/git-webserver/pages";
void zhtcp::server::SocketData::bind() 
{
    int ret = ::bind(listenfd_, (struct sockaddr*)&mAddr, sizeof(mAddr));
    if (ret == -1) {
        std::cout << "bind error in file <" << __FILE__ << "> "<< "at " << __LINE__ << std::endl;
        exit(0);
    }
}

void zhtcp::server::SocketData::listen() {
    int ret = ::listen(listenfd_, 1024);
    if (ret == -1) {
        std::cout << "listen error in file <" << __FILE__ << "> "<< "at " << __LINE__ << std::endl;
        exit(0);
    }
}
zhtcp::server::SocketData::SocketData(int port=8888,const char * ip=NULL)
 {
     bzero(&(mAddr), sizeof(mAddr));
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(port);
    if (ip != nullptr) {
        ::inet_pton(AF_INET, ip, &mAddr.sin_addr);
    } else {
        mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd_== -1){
        std::cout << "creat socket error in file <" << __FILE__ << "> "<< "at " << __LINE__ << std::endl;
        exit(0);
    }
    setReusePort(listenfd_);
   //zhtcp::setnonblocking(listenfd_);
 }

 void zhtcp::server::TcpServer::run()
 {
     std::unique_ptr<ThreadPool> pool(new zhtcp::ThreadPool(2));
     zhtcp::Epoll epoll;   
     epoll.update(socketdata_.listenfd_,false);
     //epoll.setconnectionCallback(std::bind(&zhtcp::ThreadPool::append,this,_1));
     //epoll.setcallback(std::bind(&zhtcp::server::TcpServer::do_request,this,_1));
  
     while(true)
      {
        std::vector<int>clients=epoll.poll(socketdata_.listenfd_);
        for(auto &p:clients)
        {   printf("here:65\n");
            std::shared_ptr<do_requestArgs> args(new do_requestArgs);//注意不new的话指针指针为分配地址
            args->connfd=p;args->state=1;
            pool->append(args,std::bind(&TcpServer::do_request,this,_1));

        }
          printf("here:71\n");
      }
 }

void zhtcp::server::TcpServer::do_request(/*SocketData socketdata*//*int connfd,int &connection_state*/ std::shared_ptr<void> arg)
{
    std::shared_ptr<do_requestArgs> args = std::static_pointer_cast<do_requestArgs>(arg);

    int connfd=args->connfd;
    int connection_state=args->state;
    std::cout<<"do_request"<<std::endl;
    char buffer[4096];
    memset(buffer,'\0',4096);
    int date_read=0;
    int check_index=0;
    int read_index=0;
    int start_line=0;
    status::CHECK_STATE checkstate=status::PARSE_REQUESTLINE;
    while(1)
    {
        date_read=recv(connfd,buffer+read_index,4096-read_index,0); //**
        if(date_read==-1)
           {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
               break;
            std::cout<<"error recv"<<std::endl;
           args->state=-1;
            break;
           }
        else if(date_read==0)
        {
            args->state=0;
            std::cout<<"clinet close connection"<<std::endl;
            break;//当对端关闭连接时，套接字处在可读状态，必须通过read==0来判断，如果==0，关闭套接字，从epoll移除
        }
        std::cout<<buffer<<std::endl;
        read_index+=date_read;
        zhtcp::server::HttpRequest request;
        zhtcp::server::HttpResponse response;
        status::HTTP_CODE result=zhtcp::server::parse_content(buffer,check_index,checkstate,read_index,start_line,request);
        if (result==status::NO_REQUEST)
           continue;
        else if(result==status::GET_REQUEST)
        {
           
            std::cout<<"GET_REQUEST"<<std::endl;
            header(request,response);
            getMine(request,response);
            stat_file(request,response);
            send_r(response,connfd);
            break;
        }
        else
        {
            std::cout<<"unexpected request"<<std::endl;
            break;
        }
        
    }
    std::cout<<"out while"<<std::endl;
}


void zhtcp::server::TcpServer::header(HttpRequest &request,HttpResponse &response)
{
    response.version_=request.version_;
    response.addHeader("Server", "zh webserver");

}

void zhtcp::server::TcpServer::getMine(HttpRequest &request,HttpResponse &response)
{
  std::string uri=request.uri_;
  response.filepath=uri;
  if(uri.rfind('.')!=std::string::npos)
      uri=uri.substr(uri.find('.'));
  decltype(::Mime_map)::iterator it;

    if ((it = Mime_map.find(uri)) != Mime_map.end()) {
        response.mime=it->second;// {".html", "text/html"}    Mime=“text/html“
    } else 
    {
        response.mime=Mime_map.find("default")->second;
    }
}

void zhtcp::server::TcpServer::stat_file(HttpRequest &request,HttpResponse &response)
{
   struct stat buf;
   char file[strlen(basepath) + strlen(response.filepath.c_str())+1];
    strcpy(file, basepath);
    strcat(file, response.filepath.c_str());  //C 库函数 char *strcat(char *dest, const char *src) 把 src 所指向的字符串追加到 dest 所指向的字符串的结尾
   int ret=stat(file,&buf);
   if(ret==0)
     {
        response.sstate="OK";
        response.mstate=response::k200Ok;
        response.filepath=file;
     }
     else
     {
        if(errno==ENOENT)
            {
             response.sstate="Not Found";
             response.mstate=response::k404NotFound;
            }
        else
           {
               response.sstate="Forbidden";
               response.mstate=response::k403forbiden;
           }
     }
     
};
void zhtcp::server::TcpServer::send_r(HttpResponse &response,int &connfd)
{
  char head[4096];
  memset(head,'\0',strlen(head));
  response.create_response(head);
  const char *internal_error = "Internal Error";
  struct stat stat_buffer;
  if(stat(response.filepath.c_str(),&stat_buffer)<0)
     {
         sprintf(head,"%sContent-Length: %d\r\n\r\n",head,strlen(internal_error));
         sprintf(head, "%s%s", head, internal_error);
         std::cout<<head<<std::endl;
        ::send(connfd, head, strlen(head), 0);
        return;
     }
  int filefd = ::open(response.filepath.c_str(), O_RDONLY);
    if (filefd < 0) {
        sprintf(head, "%sContent-Length: %d\r\n\r\n", head, strlen(internal_error));
        sprintf(head, "%s%s", head, internal_error);
        ::send(connfd, head, strlen(head), 0);
        return;
    }

    sprintf(head,"%sContent-Length: %d\r\n\r\n", head, stat_buffer.st_size);
    std::cout<<head<<std::endl;
    ::send(connfd, head, strlen(head), 0);//发送应答报文
    /*char INDEX_PAGE[] = "<!DOCTYPE html>\n"
                    "<html>\n"
                    "<head>\n"
                    "    <title>Welcome to LC WebServer!</title>\n"
                    "    <style>\n"
                    "        body {\n"
                    "            width: 35em;\n"
                    "            margin: 0 auto;\n"
                    "            font-family: Tahoma, Verdana, Arial, sans-serif;\n"
                    "        }\n"
                    "    </style>\n"
                    "</head>\n"
                    "<body>\n"
                    "<h1>Welcome to LC WebServer!</h1>\n"
                    "<p>If you see this page, the lc webserver is successfully installed and\n"
                    "    working. </p>\n"
                    "\n"
                    "<p>For online documentation and support please refer to\n"
                    "    <a href=\"https://github.com/MarvinLe/WebServer\">LC WebServer</a>.<br/>\n"
                    "\n"
                    "<p><em>Thank you for using LC WebServer.</em></p>\n"
                    "</body>\n"
                    "</html>";*/
    void *mapbuf = mmap(NULL, stat_buffer.st_size, PROT_READ, MAP_PRIVATE, filefd, 0);
    ::send(connfd, mapbuf,stat_buffer.st_size, 0);//发送文件内容
    munmap(mapbuf, stat_buffer.st_size);
    //send(connfd,INDEX_PAGE,sizeof(INDEX_PAGE),0);
    close(filefd);
    return;

      
};