#ifndef ZERO_THREADPOOL_H
#define ZERO_THREADPOOL_H

#include<iostream>
#include<future>
#include<mutex>
#include<queue>
#include<memory>
#include<functional>

#ifdef WIN32
#include<windows.h>
#else
#include<sys/time.h>
#endif

using namespace std;
void getnow(timeval*tv);
int64_t getNowMs();

#define TNOW getnow
#define TNOWMS getNowMs

class Zero_ThreadPool
{

protected:
    struct TaskFunc//定义一个线程的直线函数和超时时间类
    {
        TaskFunc(uint64_t expertime):_expertime(expertime){};
        std::function<void()> _func;//执行函数
        int64_t _expertime = 0;//超时的时间


    };
    typedef shared_ptr<TaskFunc> TaskFuncptr;//定义一个智能指针
public:
    Zero_ThreadPool();//构造函数
   virtual  ~Zero_ThreadPool();//析构函数
    bool init(size_t num);//初始化函数
    bool start();//开启线程池函数
    bool stop();//关闭线程池
    size_t getthreadNum() //获取线程的数量
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _threads.size();
    }

    size_t getJobNum()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _tasks.size();
    }

    /*
    *
    * @brief 用线程池启用任务(F是function, Args是参数)
    *
    * @param ParentFunctor
    * @param tf
    * @return 返回任务的future对象, 可以通过这个对象来获取返回值
    */
    template<class F, class ...Args>
    auto exec(F&& f, Args&&...args)->std::future<decltype(f(args...))>
    {
        return exec(0,f,args...); //采用同名函数来递归解析变长参数
    }

    template<class F, class...Args>
    auto exec(int64_t timeMs, F&& f,Args&&args)->std::future<decltype(f(args...))>
    {
        int64_t expertime = (timeMs == 0 ? 0:timeMs+ TNOWMS);//获取超时时间
        using Rettype = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<Rettype()>>(std::bind(std::forward<F>(f),std::forward<Args>(args...)));
        TaskFuncptr fptr = std::make_shared<TaskFunc>(expertime); //封装任务指针，设置过期时间
        fptr->_func = [task](){(*task)()};
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.push(fptr);
        _condition.notify_one();
        return task.get_future();   
    }

    //等待所有的工作全部结束
    bool WaitForAllDone(int millsecond=-1);

protected:
    bool get(TaskFuncptr&task);//获取正在执行的任务
    bool isTerminate(){return _terminate};//获取线程池是否终止了
    void run();

protected:
    queue<TaskFuncptr>  _tasks;//存储所有的任务队列
    std::vector<std::thread*>_threads;//存储所有的线程
    std::condition_variable _condition;//条件变量
    size_t thread_num;//线程数量
    bool _terminate;//是否终止线程池
    std::atomic<int> _atomic{0}; //原子操作

};




#endif
