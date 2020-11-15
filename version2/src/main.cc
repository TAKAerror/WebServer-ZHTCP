#include"../include/epoll.h"
#include"../include/tcpServer.h"
int main(int argc,char ** argv)
{
  zhtcp::server::TcpServer tcpserver(8888,NULL);
  tcpserver.run();
  
}