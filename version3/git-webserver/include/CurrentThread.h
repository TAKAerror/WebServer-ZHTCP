#pragma once
#include<pthread.h>
#include<sys/syscall.h>//sys_gettid
#include <unistd.h>//syscall
namespace zhtcp
{
namespace CurrentThread //存储当前线程的一些数据（私有）
{
  // internal
 extern  __thread int t_cachedTid;
 
  void cacheTid();

  inline int tid()
  {
    if (t_cachedTid == 0)
    {
     t_cachedTid=static_cast<pid_t>(::syscall(SYS_gettid));
    }
    return t_cachedTid;
  }
}
}