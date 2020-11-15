#pragma once
#include"mutexLock.h"
#include<pthread.h>
namespace zhtcp
{
    class Conndition
    {
      public:
         Conndition(MutexLock & mutexlock):mutexlock_(mutexlock)
         {pthread_cond_init(&connd_,NULL);}
         void wait(){pthread_cond_wait(&connd_,mutexlock_.getLock());}  //该函数貌似没解锁
         void notify(){pthread_cond_signal(&connd_);}
         void notifyAll() {
        pthread_cond_broadcast(&connd_);
    }
         ~Conndition()
         {
             pthread_cond_destroy(&connd_);
         }
      private:
        MutexLock &mutexlock_;//忘记加&了，血泪啊
        pthread_cond_t connd_;
         

    };
}