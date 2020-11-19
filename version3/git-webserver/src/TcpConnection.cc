#include"../include/TcpConnection.h"
#include <sys/types.h>  
 #include <sys/socket.h>
 #include"../include/httpRequest.h"
#include"../include/Channel.h"
#include<cstdlib>
#include<functional>
zhtcp::TcpConnection::TcpConnection(EventLoop * loop,int fd):eventloop_(loop),fd_(fd),channel_(new Channel(fd,loop))
{
  channel_->setReadcallback_(std::bind(&TcpConnection::handread,this));
}
void zhtcp::TcpConnection::establishRead()
{
  channel_->enableRead();
}
void zhtcp::TcpConnection::handread()
{
   //char * buf;
   printf("TcpConnection:handRead\n");
  
   std::shared_ptr<zhtcp::server::do_requestArgs> args(new zhtcp::server::do_requestArgs);//注意不new的话指针指针为分配地址
   args->connfd=fd_;args->state=1;
   readcallback_(args);
   if (args->state == -1)//connection_state传递了在writecallback_里redv返回0，redv返回-1的状态
	 ERR_EXIT("read");
   if (args->state== 0)
	{
	 std::cout<<"client close"<<std::endl;
     channel_->disableAll();
	}
	
  
}