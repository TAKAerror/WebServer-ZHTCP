#pragma once
#include<pthread.h>
#include<stdio.h>
namespace zhtcp
{
    class MutexLock
    {
      public:
        MutexLock()
        {
          pthread_mutex_init(&mutex_,NULL);
        }
        void lock()
        {
          //printf("lock\n");
          pthread_mutex_lock(&mutex_);
        }
        void unlock()
        {
          //printf("unlock\n");
          pthread_mutex_unlock(&mutex_);
        }
        pthread_mutex_t *getLock()
        {
          return &mutex_;
        }
        ~MutexLock()
        {
          pthread_mutex_destroy(&mutex_);
        }
      private:
         pthread_mutex_t mutex_;

    };
    class MutexGuaundLock
    {
      public:
        MutexGuaundLock(MutexLock& mutexLock) :mutexlock_(mutexLock)
        {
         mutexlock_.lock();
        }
        ~MutexGuaundLock()
        {
          mutexlock_.unlock();
        }
      private:
        MutexLock &mutexlock_;
    };
}