#pragma once
#include<sys/epoll.h>
#include<unistd.h>
#include<vector>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include<iostream>
#include<functional>
#include"httpRequest.h"
#include"threadPool.h"
#include<vector>
#include"Channel.h"
#include<map>
//#include"tcpServer.h"
namespace zhtcp
{
    #define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)
class Channel;
class Epoll
{
    //typedef std::function<void(zhtcp::server::do_requestArgs)> Writecallback;
    //typedef std::function<void(zhtcp::Task)>connectionCallback;
    public:
       Epoll():events_(MAX_EVENTS)
       {
        epollfd_ = ::epoll_create(MAX_EVENTS);
        if (epollfd_ == -1) 
        {std::cout << "epoll create error" << std::endl;
        exit(-1);
        }
         
       }
       ~Epoll()
       {
           ::close(epollfd_);
       };
        void update(int fd,bool enable_et);
        void update(zhtcp::Channel * channel);
        void erase();
        void modfd();
        std::vector<int> poll(int listenfd);
        void poll(std::vector<Channel *> *channellist);
        int  setnonblocking(int fd);
       // void setcallback(Writecallback cb){writecallback_=cb;};//利用回调函数处理request、response
       //void setconnectionCallback(connectionCallback cb){connectioncallback_=cb;};
    private:
    int epollfd_;
    //int idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);//空文件描述符
    static const int MAX_EVENTS;
    typedef std::vector<struct epoll_event> EventList;
    EventList events_;
    std::vector<int> clients;
    std::vector<Channel *>channellist_;
    std::map<int,Channel*>channelmap_;
    //Writecallback writecallback_;
    //connectionCallback connectioncallback_;
    //std::vector<zhtcp::Task> taskqueu_;
};
}