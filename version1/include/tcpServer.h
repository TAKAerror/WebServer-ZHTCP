#pragma once
//#include"httpRequest.h"
//#include"httpResponse.h"
#include<functional>
#include<sys/socket.h>
#include<netinet/in.h>
#include"epoll.h"
#include"httpRequest.h"
#include"httpResponse.h"
namespace zhtcp
{
 /* int setnonblocking(int fd)
{
  int oldoption=fcntl(fd,F_GETFL);
  int newoption=oldoption | O_NONBLOCK;
  fcntl(fd,F_SETFL,newoption);
  return oldoption;
}*/
    namespace server
    {
        class SocketData
        {
          public:
            SocketData(int port,const char *ip);
            void setListenfd(int listenfd) {listenfd_=listenfd;};
            void bind();
            void listen();
            int listenfd_;
            int connfd_;
            struct sockaddr_in cliaddr_,mAddr; 
           void setReusePort(int fd) 
               {
                 int opt = 1;
                 setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
               }
            int port_;
            const char * ip_;
        };
        typedef std::function<void ()> MessageCallback;
        class TcpServer
        {
          public:
             TcpServer(/*MessageCallback cd,*/int port,const char * ip):socketdata_(port,ip)
               {
                 socketdata_.bind();
                 socketdata_.listen();
                 //this->setMessageCallback(cd);
               }
               void setMessageCallback(MessageCallback cd){messagecallback_=cd;};
               void run();//放listenfd，绑定回调，poll
               static void header(HttpRequest & request,HttpResponse &response);//从request传递对应的参数到response
               static void getMine(HttpRequest & request,HttpResponse &response);
               static void stat_file(HttpRequest & request,HttpResponse &response);
               void send_r(HttpResponse &response,int & fd);
               void do_request(int fd,int & connection_state);//传递给epoll的writecallback_
          private:
            Epoll epoll_;
            SocketData socketdata_;
            MessageCallback messagecallback_;
            

              
        };
    }
}