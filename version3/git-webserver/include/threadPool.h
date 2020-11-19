#pragma once
#include<pthread.h>
#include"mutexLock.h"
#include"conndition.h"
#include<functional>
#include<queue>
#include<vector>
#include<memory>
#include"../include/httpRequest.h"
namespace zhtcp
{
    
    struct Task
    {
         std::function<void(std::shared_ptr<void>)> runinloop_;
         std::shared_ptr<void> args_;
          
    };
    class ThreadPool
    {
        
     public:
        ThreadPool(int numthreads);
        static void * work(void * args); 
        void run();
        void append(std::shared_ptr<void> arg,std::function<void(std::shared_ptr<void>)> fun);
     private:
        int numthreads_;
        MutexLock  mutexlock_;
        Conndition conndition_;   //payattention
        std::queue<Task>  runinlooplist;
        std::vector<pthread_t> threads;
    };
}