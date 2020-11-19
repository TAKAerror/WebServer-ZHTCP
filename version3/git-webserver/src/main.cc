#include"../include/epoll.h"
#include"../include/tcpServer.h"
#include"../include/EventLoop.h"
int main(int argc,char ** argv)
{
  zhtcp::EventLoop loop;
  zhtcp::server::TcpServer tcpserver(8888,NULL,&loop,"server");
  tcpserver.run();
  loop.loop();
  
}