#include "../include/signal_test/libraries.h"

std::mutex lock;

void checker_function(int id,std::string name,int delay)
{
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout<<"Init "<<name<<" function: "<<std::ctime(&now_time)<<std::endl;
    double lower_boundn = 23.0;
    double upper_boundn = 30.0;

    while(1)
    {
        lock.lock();
        std::cout<<"Checking"<<std::endl;
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

int main(int argc, char** argv)
{
    std::cout<<"Concurrent program started"<<std::endl;
    std::cout<<"Press \"q\" to end testing"<<std::endl;
    // std::thread th1(thread_function,"thread", 1,lock);
    std::thread ch1(checker_function,1,"checker",1000);
    // th1.join();
    ch1.join();
    return 0;
}