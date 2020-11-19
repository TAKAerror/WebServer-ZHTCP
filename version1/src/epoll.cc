#include"../include/epoll.h"
#include<sys/epoll.h>
#include<iostream>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iterator>
#include<cstdio>
#include<vector>
#include<string.h>
#include<algorithm>
const int zhtcp::Epoll::MAX_EVENTS = 10000;
epoll_event *zhtcp::Epoll::events;

int zhtcp::Epoll::setnonblocking(int fd)
{
  int oldoption=fcntl(fd,F_GETFL);
  int newoption=oldoption | O_NONBLOCK;
  fcntl(fd,F_SETFL,newoption);
  return oldoption;
}
void zhtcp::Epoll::update(int fd,bool enable_et)
{
    epoll_event event;
    event.data.fd=fd;
    event.events=EPOLLIN;
    if(enable_et)
    {
        event.events |=EPOLLET;
    }
    epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&event);
	clients.push_back(fd);
    setnonblocking(fd);
    
};
void zhtcp::Epoll::modfd()
{

};
void zhtcp::Epoll::poll(int listenfd)
{
    epoll_event events[MAX_EVENTS];
    int nready=epoll_wait(epollfd_,events,MAX_EVENTS,-1);
    socklen_t peerlen;
    struct sockaddr_in peeraddr;
    epoll_event event;
    int connfd;
    for (int i = 0; i < nready;++i)
		{
			if (events[i].data.fd == listenfd)
			{
				peerlen = sizeof(peeraddr);
				connfd = ::accept4(listenfd, (struct sockaddr*)&peeraddr,
						&peerlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

				if (connfd == -1)
				{
					if (errno == EMFILE)
					{
						close(idlefd);
						idlefd = accept(listenfd, NULL, NULL);
						close(idlefd);
						idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);
						continue;
					}
					else
						ERR_EXIT("accept4");
				}


				std::cout<<"ip="<<inet_ntoa(peeraddr.sin_addr)<<
					" port="<<ntohs(peeraddr.sin_port)<<std::endl;

				
				
				update(connfd,false);
			}
			else if (events[i].events & EPOLLIN)
			{
				connfd = events[i].data.fd;
				int connection_state=1;
				if (connfd < 0)
					continue;

				char buf[1024] = {0};
				writecallback_(connfd,connection_state);
				//int ret = read(connfd, buf, 1024);
				
				if (connection_state == -1)//connection_state传递了在writecallback_里redv返回0，redv返回-1的状态
					ERR_EXIT("read");
				if (connection_state == 0)
				{
					std::cout<<"client close"<<std::endl;
					close(connfd);
					event = events[i];
					epoll_ctl(epollfd_, EPOLL_CTL_DEL, connfd, &event);
					clients.erase(std::remove(clients.begin(), clients.end(), connfd), clients.end());
					continue;
				}
				else
				    {close(connfd);
					event = events[i];
					epoll_ctl(epollfd_, EPOLL_CTL_DEL, connfd, &event);
					clients.erase(std::remove(clients.begin(), clients.end(), connfd), clients.end());
					}
				std::cout<<buf;
				write(connfd, buf, strlen(buf));
			}
		}
}
