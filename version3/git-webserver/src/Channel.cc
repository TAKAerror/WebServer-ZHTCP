#include"../include/Channel.h"
#include<pthread.h>
#include<sys/epoll.h>
const int zhtcp::Channel::kNoneEvent = 0;
const int zhtcp::Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int zhtcp::Channel::kWriteEvent = EPOLLOUT;
zhtcp::Channel::Channel(int fd,EventLoop * loop):
fd_(fd),
index_(-1),
event_(0),
revent_(0),
eventloop_(loop)
{
};

void zhtcp::Channel::update()
{
 eventloop_->update(this);
}

void zhtcp::Channel::handEvent()
{  
    printf("handread\n");
    if(revent_&EPOLLIN)
      {
         printf("handread\n");
           readcallback_();
      }
}