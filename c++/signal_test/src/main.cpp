#include "../include/signal_test/libraries.h"
#include "../include/signal_test/json/json.h"

std::mutex lock;

void thread_function(int id,std::string name,int delay)
{
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout<<"Init "<<name<<" function: "<<std::ctime(&now_time)<<std::endl;
    double i = 20;
    while(1)
    {
    // ------------------------------------
        //Temperature sensor simulation
        //Replace with your signal processing code
        i += 1;
        if (i > 25)
            i = 20;
        Json::Value value_obj;
        value_obj["signal"] = i;
    // ------------------------------------ 
        lock.lock();   
        std::ofstream outputFileStream;
        outputFileStream.open("../data.json");
        if( !outputFileStream ) { // file couldn't be opened
            std::cerr << "Error: file could not be opened" << std::endl;
            exit(1);
        }
        outputFileStream << value_obj << std::endl;
        outputFileStream.close();
        lock.unlock();
        // --------------------------------
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}
void checker_function(int id,std::string name,int delay)
{
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout<<"Init "<<name<<" function: "<<std::ctime(&now_time)<<std::endl;
    double lower_bound = 23.0;
    double upper_bound = 30.0;

    while(1)
    {
        lock.lock();
        std::ifstream data_file("../data.json", std::ifstream::binary);
        Json::Value data;
        data_file >> data;
        data_file.close();
        lock.unlock();
        std::cout<<"measured signal: "<<data["signal"]<<'\n';
        double signal_value = data["signal"].asDouble();
        if((signal_value >= lower_bound) && (signal_value <= upper_bound))
        {
            std::cout<<"WITHIN BOUNDS ✅\n";
        }
        else
        {
            std::cout<<"OUT OF BOUNDS ❌\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

int main(int argc, char** argv)
{
    std::cout<<"Concurrent program started"<<std::endl;
    std::cout<<"Press \"q\" to end testing"<<std::endl;
    std::thread th1(thread_function,1,"thread", 1000);
    std::thread ch1(checker_function,2,"checker",1000);
    th1.join();
    ch1.join();
    return 0;
}