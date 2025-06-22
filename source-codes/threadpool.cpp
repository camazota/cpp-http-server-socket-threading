#include "threadpool.h"

threadpool::threadpool(size_t threadCount) : stop(false)
{
    for (size_t i = 0; i < threadCount; ++i)
    {
        workers.emplace_back([this]
                             {
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock lock(queue_mutex);
                    condition.wait(lock,[this]{
                        return stop || !tasks.empty();
                    });

                    if(stop && tasks.empty()){
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            } });
    }
}

void threadpool::enqueue(std::function<void()> task)
{
    {
        std::lock_guard lock(queue_mutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

threadpool::~threadpool()
{
    stop = true;
    condition.notify_all();
    for (std::thread &worker : workers)
    {
        if (worker.joinable())
            worker.join();
    }
}