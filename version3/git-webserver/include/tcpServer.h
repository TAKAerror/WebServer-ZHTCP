#pragma once
//#include"httpRequest.h"
//#include"httpResponse.h"
#include<functional>
#include<sys/socket.h>
#include<netinet/in.h>
#include<memory>
#include"threadPool.h"
#include"EventLoop.h"
#include"epoll.h"
#include"httpRequest.h"
#include"httpResponse.h"
#include"Channel.h"
#include<map>
#include"TcpConnection.h"
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
        extern char INDEX_PAGE[];
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
            int setnonblocking(int fd) 
               {
                int old_option = fcntl(fd, F_GETFL);
                int new_option = old_option | O_NONBLOCK;
                fcntl(fd, F_SETFL, new_option);
                return old_option;
               }

            int port_;
            const char * ip_;
        };
        typedef std::function<void ()> MessageCallback;
        class TcpServer
        {
          public:
             TcpServer(/*MessageCallback cd,*/int port,const char * ip,EventLoop * loop,const std::string& name):socketdata_(port,ip),eventloop_(loop),name_(name)
               {
                 channel_=(new Channel(socketdata_.listenfd_,loop));
                 socketdata_.bind();
                 socketdata_.listen();
                 channel_->setReadcallback_(std::bind(&zhtcp::server::TcpServer::newConnection,this));
                 idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);
                 
                 //channel_->enableRead();
                ;
                 //this->setMessageCallback(cd);
               }
               void newConnection();
               void setMessageCallback(MessageCallback cd){messagecallback_=cd;};
               void run();//放listenfd，绑定回调，poll
               static void header(HttpRequest & request,HttpResponse &response);//从request传递对应的参数到response
               static void getMine(HttpRequest & request,HttpResponse &response);
               static void stat_file(HttpRequest & request,HttpResponse &response);
               void send_r(HttpResponse &response,int & fd);
               void do_request( std::shared_ptr<void> args);//传递给epoll的writecallback_
               void createTask(int connfd,int state);
          private:
            Epoll epoll_;
            int idlefd ;
            SocketData socketdata_;
            MessageCallback messagecallback_;
            EventLoop * eventloop_;
            Channel * channel_;
            std::string name_;
            std::map<std::string, std::shared_ptr<zhtcp::TcpConnection> > connectionmap_;
            

              
        };
    }
}