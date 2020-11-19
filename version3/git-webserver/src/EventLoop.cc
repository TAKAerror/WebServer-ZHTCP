#include"../include/EventLoop.h"
#include<pthread.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include"../include/Channel.h"
#include<sys/socket.h>
#include"../include/mutexLock.h"
#include<iterator>
namespace zhtcp
{
 __thread EventLoop * inThisLoop=NULL;
 int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    printf( "Failed in eventfd");
    abort();//跳出
  }
  return evtfd;
};
}

void zhtcp::EventLoop::update(Channel * channel)
{
    epoll_->update(channel);
}

zhtcp::EventLoop::EventLoop():quit_(false),looping_(false),epoll_(new zhtcp::Epoll), wakeFd_(createEventfd()),wakeUpChannel_(new Channel(wakeFd_,this)),tid_(zhtcp::CurrentThread::tid()),callingFunc_(false)
{
  wakeUpChannel_->setReadcallback_(std::bind(&zhtcp::EventLoop::readWakeUp,this));
  wakeUpChannel_->enableRead();
}

void zhtcp::EventLoop::readWakeUp()
{
    uint64_t one = 1; 
    ssize_t n = read(wakeFd_, &one, sizeof one);
     if (n != sizeof one)
  {
    printf("EventLoop::handleRead() reads %d bytes instead of 8",n);
  }
}
void zhtcp::EventLoop::wakeUp()
{
  uint64_t one = 1;
  ssize_t n = write(wakeFd_, &one, sizeof one);//利用wakeupFd_唤醒很可能阻塞在poll的EventLoop
  printf("wakeUp\n");
  if (n != sizeof one)
  {
    printf("EventLoop::wakeup() writes  n  bytes instead of 8",n);
  }
}

void zhtcp::EventLoop::runInLoop(Func cb)
{
  if (isInLoop())
  {
    printf("inLoop,direct execution runInLoop\n");
    cb();
  }
  else
  {
    queueInLoop(std::move(cb));
  }
}

void zhtcp::EventLoop::queueInLoop(Func cb)
{
  {
  zhtcp::MutexGuaundLock lock(mutex_);
  funclist_.push_back(std::move(cb));
  }

  if (!isInLoop()||callingFunc_)
  {
    wakeUp();
  }
}

void zhtcp::EventLoop::runFuncList()
{
   std::vector<Func> functors;
   callingFunc_=true;
  {
  MutexGuaundLock lock(mutex_);
  functors.swap(funclist_);
  }

  for (const Func& functor : functors)
  {
    printf("func\n");
    functor();
  }
    callingFunc_= false;
}
void zhtcp::EventLoop::loop()
{
  looping_=true;
  quit_=false;
  while(!quit_)
  {
      chanelist_.clear();//用完清一清
      epoll_->poll(&chanelist_);
      printf("poll\n");
       for (Channel* channel : chanelist_)
    {
      printf("eventloop fd:%d\n",channel->fd());
      currentChannel_ = channel;
      currentChannel_->handEvent();//根据channel的revent进行调用；
    }
    runFuncList(); 
    printf("next poll\n");
  
  }
}