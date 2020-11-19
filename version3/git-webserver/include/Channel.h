#pragma once
#include<functional>
#include"EventLoop.h"
#include"httpRequest.h"
namespace zhtcp
{
    class EventLoop;
    class Channel
    {
     typedef std::function<void()> ReadCallback;   
        public:
        Channel(int fd,EventLoop * loop);
        //~Channel();
        void handEvent();
        //void handRead();
        void setEvent(int event){event=event_;};
        void setRevent(int revent){revent_=revent;};
        void setFd(int fd){fd_=fd;}
        void setIndex(int index){index_=index;};
        void update();
        void setReadcallback_(ReadCallback cb){readcallback_=cb;};
        void enableRead(){event_|=kReadEvent;update();};
        bool isWriting() const { return event_ & kWriteEvent; }
        bool isReading() const { return event_ & kReadEvent; }
        int index(){return index_;};
        void disableAll() { event_= kNoneEvent; update(); }
        int fd(){return fd_;};
        bool isNoneEvent() const { return event_== kNoneEvent; }
        int event(){return event_;};
        private:
          static const int kNoneEvent;
          static const int kReadEvent;
          static const int kWriteEvent;
          int fd_;
          int index_;//give epoll
          ReadCallback readcallback_;//bind handread,which is in TcpConnection 
          EventLoop * eventloop_;  
          int event_;
          int revent_;

    };
}