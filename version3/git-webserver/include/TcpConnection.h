#pragma once
#include"EventLoop.h"
#include"httpRequest.h"
#include"memory"
#include"Channel.h"
namespace zhtcp
{
    class TcpConnection
    {
        typedef std::function<void (std::shared_ptr<void>)> ReadCallback; 
        public:
          TcpConnection(EventLoop * eventloop,int fd);
         // ~TcpConnection();
          void setReadcallback(const ReadCallback &cb){readcallback_=cb;};
          void handread();
          void establishRead();
          void setArgs(std::shared_ptr<zhtcp::server::do_requestArgs> arg){args_=arg;};
        private:
           EventLoop * eventloop_;
           ReadCallback readcallback_;
           //std::shared_ptr<Channel> channel_;//用这个channel_不能随便赋给其他Channel *
           Channel * channel_;
           std::shared_ptr<void> args_;
           int fd_;

    };
}