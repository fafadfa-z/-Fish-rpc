#include <iostream>
#include <thread>
#include "base/timer.h"



using namespace std;

 void fun(int time)
 {
     cout<< time<<endl;


 }


int main()
{
    auto timer = Fish::Timmer::init(1000);

    timer->addPriodTask(fun,1000,1);

    timer->start();

    while(1)
    {
        std::this_thread::sleep_for(1s);
    }


    return 0;
}