#include "zero_theradpool.h"
#include<iostream>
#include<mutex>

using namespace std;


Zero_ThreadPool::Zero_ThreadPool():_terminate(false),thread_num(1)
{


}

Zero_ThreadPool::~Zero_ThreadPool()
{

    stop();
}

//初始化函数
bool Zero_ThreadPool::init(size_t num)
{
    std::unique_lock<std::mutex> lock(_mutex);
    if(!_threads.empty())
    {
        return false;
    }
    return true;
}