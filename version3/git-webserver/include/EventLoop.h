#pragma once
#include"Channel.h"
#include"epoll.h"
#include<pthread.h>
#include<memory.h>
#include"threadPool.h"
#include<vector>
#include"CurrentThread.h"
#include<functional>
#include"mutexLock.h"
namespace zhtcp
{
    class Epoll;
    class Channel;
    class ThreadPool;
    class EventLoop
    {public:
        typedef std::function<void ()> Func;
        EventLoop();
        //~EventLoop();
        void loop();
        void wakeUp();
        void readWakeUp();
        void update(Channel *channel);
        bool assertInLoop(){ return (CurrentThread::tid()==tid_);};
        void runInLoop(Func cd);
        void queueInLoop(Func cb);
        bool isInLoop(){ return tid_==zhtcp::CurrentThread::tid();}
        void runFuncList();
      private:
         std::unique_ptr<ThreadPool> threadpool_=NULL;
         std::unique_ptr<zhtcp::Epoll> epoll_=NULL;
         bool looping_;
         bool quit_;
         int wakeFd_;
         Channel * currentChannel_;
         std::vector<Channel *>chanelist_;
         pid_t tid_;
         std::vector<Func> funclist_;
         MutexLock mutex_;
         std::unique_ptr<Channel> wakeUpChannel_;
         bool callingFunc_;

    };
}