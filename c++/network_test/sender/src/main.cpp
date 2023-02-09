#include "../include/sender_test/libraries.h"
#include "../include/sender_test/json/json.h"

std::mutex lock;
std::atomic_bool stop_threads = false;
#define PORT 8080
#define IP_TO_SEND "127.0.0.1"
#define MAX_SIZE 1024

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
    
    // socket
    char* str;
    int fd = 0;
    struct sockaddr_in serverAddr;

    
    // ------------------------------------
    
    while(1)
    {
            // socket -----------------------------
        fd = socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0)
        {
            printf("Error : Could not create socket\n");
            return;
        }
        else
        {
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(PORT);
            serverAddr.sin_addr.s_addr = inet_addr(IP_TO_SEND);
            memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
        }
        if (connect(fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) >= 0)
        {
            // ------------------------------------
            // temperature sensor simulation
            // replace with your signal processing code
                i += 1;
                if (i > 25)
                    i = 20;
                Json::Value value_obj;
                value_obj["signal"] = i;
            // ------------------------------------ 
            // writing to data.json
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
            // sending
            char temp_buff[MAX_SIZE];
            Json::FastWriter fastWriter;
            std::string json_string = fastWriter.write(value_obj);
            const char * message = json_string.c_str();
            if (strcpy(temp_buff, message) == NULL)
            {
                perror("strcpy");
                return;
            }

            if (write(fd, temp_buff, strlen(temp_buff)) == -1)
            {
                perror("write");
                return;
            }

            printf("Written data\n");

            if (stop_threads)
            {
                close(fd);
                return;
            }
        }
        else
        {
            std::cout<<"server is not connected\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

int main(int argc, char** argv)
{
    std::cout<<"Concurrent program started"<<std::endl;
    std::cout<<"Press \"q\" to end testing"<<std::endl;
    std::thread th1(thread_function,1,"thread", 1000);
    std::thread ip1(input_function,2,"input",10);
    th1.join();
    ip1.join();
    return 0;
}