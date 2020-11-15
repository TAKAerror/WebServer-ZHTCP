#include"../include/threadPool.h"
#include<pthread.h>
zhtcp::ThreadPool::ThreadPool(int numthreads=3):numthreads_(numthreads),conndition_(mutexlock_)
        {
            printf("threads:%d\n",numthreads);
            threads.resize(numthreads);
            for(int i=0;i<numthreads;i++)
             {
               int res=pthread_create(&threads[i],NULL,work,this);//work+static
               if(res!=0)
                  printf("error in pthread_create");
             }
        }
 void  * zhtcp::ThreadPool::work(void * args)
        {
          ThreadPool *pool = static_cast<ThreadPool *>(args);
          if(pool==NULL)
             printf("error pool");
          else
          {
            pool->run();
          }
          
        }
void zhtcp::ThreadPool::run()
        { 
         while(true)
         {  //MutexGuaundLock guard(this->mutexlock_);
            mutexlock_.lock();
            printf("thread run\n");
            while(runinlooplist.empty())//不知道为什么conndition的wait函数没有解锁，导致与append造成死锁
              conndition_.wait();
           printf("thread runing\n");
           zhtcp::Task task;
           task=runinlooplist.front();
           runinlooplist.pop();
           mutexlock_.unlock();
           task.runinloop_(task.args_);
         }
        }
void zhtcp::ThreadPool::append(std::shared_ptr<void> arg,std::function<void(std::shared_ptr<void>)> fun)
{
    printf("append\n");
    MutexGuaundLock graund(this->mutexlock_);
    zhtcp::Task task;
    task.args_=arg;
    task.runinloop_=fun;
    this->runinlooplist.push(task);
    this->conndition_.notify();
    printf("notify\n");
    return;
}
        