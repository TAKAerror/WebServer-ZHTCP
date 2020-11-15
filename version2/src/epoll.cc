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
#include"../include/httpRequest.h"
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
	printf("fd:%d\n",fd);
	clients.push_back(fd);
    setnonblocking(fd);
    printf("update\n");
};
void zhtcp::Epoll::modfd()
{

};
std::vector<int> zhtcp::Epoll::poll(int listenfd)
{
	printf("enter poll\n");
    epoll_event events[MAX_EVENTS];
    int nready=epoll_wait(epollfd_,events,MAX_EVENTS,-1);
    socklen_t peerlen;
    struct sockaddr_in peeraddr;
    epoll_event event;
    int connfd;
	std::vector<int>clientfd;
	printf("nreadt:%d\n",nready);
    for (int i = 0; i < nready;++i)
		{
			
			if (events[i].data.fd == listenfd)
			{
				peerlen = sizeof(peeraddr);
				connfd = ::accept4(listenfd, (struct sockaddr*)&peeraddr,
						&peerlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

				if (connfd == -1)
				{
					/*if (errno == EMFILE)
					{
						close(idlefd);
						idlefd = accept(listenfd, NULL, NULL);
						close(idlefd);
						idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);
						continue;
y
					else*/
						ERR_EXIT("accept4");
				}


				std::cout<<"ip="<<inet_ntoa(peeraddr.sin_addr)<<
					" port="<<ntohs(peeraddr.sin_port)<<std::endl;

				
				
				update(connfd,false);
			}
			else if (events[i].events & EPOLLIN)
			{
				if(events[i].events&EPOLLRDHUP|| (events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLHUP))//短连接关闭时，epoll收到什么事件？无法用这种方法应对短连接
				   {
					std::cout<<"client close"<<std::endl;
					close(connfd);
					event = events[i];
					epoll_ctl(epollfd_, EPOLL_CTL_DEL, connfd, &event);
					clients.erase(std::remove(clients.begin(), clients.end(), connfd), clients.end());
					continue;
				   }
				else
				{printf("94\n");
				connfd = events[i].data.fd;
				//int connection_state=1;
				if (connfd < 0)
					continue;
                //printf("here:99\n"); 
				auto it=find(clients.begin(),clients.end(),connfd);printf("connfd:%d\n",connfd);
                if(it!=clients.end())
                     {
						 //printf("here:103\n");
					  clientfd.push_back(*it);
					 // printf("here:105\n");
					  clients.erase(it);
					 }
				else
				{
					printf("close connfd\n");
					//close(connfd);  //不能直接关闭，考虑主线程子线程并行问题：子线程用connfd时主线程close了connfd；fix:添加一个定时器，多少秒之后关闭connfd
					event = events[i];
					epoll_ctl(epollfd_, EPOLL_CTL_DEL, connfd, &event);
				}
				//int ret = read(connfd, buf, 1024);
				
				/*if (connection_state == -1)//connection_state传递了在writecallback_里redv返回0，redv返回-1的状态
					ERR_EXIT("read");*/
				/*if (connection_state == 0)
				{
					std::cout<<"client close"<<std::endl;
					close(connfd);
					event = events[i];
					epoll_ctl(epollfd_, EPOLL_CTL_DEL, connfd, &event);
					clients.erase(std::remove(clients.begin(), clients.end(), connfd), clients.end());
					continue;
				}*/

				//std::cout<<buf;
				//write(connfd, buf, strlen(buf));
				
				}
			}
		}
		printf("out poll\n");
		return clientfd;
}
