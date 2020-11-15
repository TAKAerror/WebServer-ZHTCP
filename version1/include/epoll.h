#pragma once
#include<sys/epoll.h>
#include<unistd.h>
#include<vector>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include<iostream>
#include<functional>
//#include"tcpServer.h"
namespace zhtcp
{
    #define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

class Epoll
{
    typedef std::function<void(int,int&)> Writecallback;
    public:
       Epoll()
       {
        epollfd_ = ::epoll_create(MAX_EVENTS);
        if (epollfd_ == -1) 
    
        {std::cout << "epoll create error" << std::endl;
        exit(-1);
        }
        events = new epoll_event[MAX_EVENTS];

       }
       ~Epoll()
       {
           ::close(epollfd_);
       };
        void update(int fd,bool enable_et);
        void erase();
        void modfd();
        void poll(int listenfd);
        int  setnonblocking(int fd);
        void setcallback(Writecallback cb){writecallback_=cb;};//利用回调函数处理request、response
    private:
    int epollfd_;
    int idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);//空文件描述符
    static const int MAX_EVENTS;
    static epoll_event *events;
    std::vector<int> clients;
    Writecallback writecallback_;
};
}