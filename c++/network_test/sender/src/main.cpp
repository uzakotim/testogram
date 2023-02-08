#include "../include/signal_test/libraries.h"
#include "../include/signal_test/json/json.h"

std::mutex lock;
std::atomic_bool stop_threads = false;

void input_function(int id,std::string name,int delay)
{   
    lock.lock();
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout<<"Init "<<name<<" function: "<<std::ctime(&now_time);
    lock.unlock();
    struct termios old_tio, new_tio;
    unsigned char c;

    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO,&old_tio);
    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;
    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &=(~ICANON & ~ECHO);
    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
    while(1)
    {               
        c=getchar();
        if (c == 'q')
            break;
        std::cout<<c<<'\n';  
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };
    /* restore the former settings */
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
    stop_threads = true;
    return;
}

void thread_function(int id,std::string name,int delay)
{
    lock.lock();
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout<<"Init "<<name<<" function: "<<std::ctime(&now_time);
    lock.unlock();

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
        if (stop_threads)
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}
void checker_function(int id,std::string name,int delay)
{
    lock.lock();
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout<<"Init "<<name<<" function: "<<std::ctime(&now_time);
    lock.unlock();
    
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
        if (stop_threads)
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

int main(int argc, char** argv)
{
    std::cout<<"Concurrent program started"<<std::endl;
    std::cout<<"Press \"q\" to end testing"<<std::endl;
    std::thread th1(thread_function,1,"thread", 1000);
    std::thread ch1(checker_function,2,"checker",1000);
    std::thread ip1(input_function,3,"input",10);
    th1.join();
    ch1.join();
    ip1.join();
    return 0;
}