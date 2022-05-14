#include "zero_threapool.h"
#include <iostream>

using namespace std;

void func0()
{
    std::cout << "func0" <<std::endl;
}

void func1(int a)
{
    std::cout << "func1 a=" << a <<std::endl;
}

void func2(int a,string b)
{
    std::cout << "func2() a=" <<a << " b =" << b <<std::endl;
}

void test1()
{
    Zero_ThreadPool threadpool;
    threadpool.init(2);
    threadpool.start();
    threadpool.exec(1000,func0);
    threadpool.exec(func1, 10);
    threadpool.exec(func2, 20, "test1");
    threadpool.WaitForAllDone();
    threadpool.stop();
}


int func1_future(int a)
{
    std::cout <<"func1_future a =" << a <<std::endl;
    return a; 
}  

string func2_future(int a, string b)
{
    std::cout << "func_future2 a=" << a << " b=" << b << endl;
    return b;
}

void test2()
{
    Zero_ThreadPool threadpool;
    threadpool.init(2);
    threadpool.start();
    std::future<decltype(func1_future(0))> result1 = threadpool.exec(func1_future,10);
    std::future<string> result2 = threadpool.exec(func2_future,20,"func2_future");
    std::cout << "result1"  << result1.get() <<std::endl;
    std::cout << "result2 " << result2.get() << std::endl;
    threadpool.WaitForAllDone();
    threadpool.stop();   
}

int main()
{
    //test1();
    test2();
    return 0;

}