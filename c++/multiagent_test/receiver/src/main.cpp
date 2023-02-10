#include "../include/receiver_test/libraries.h"
#include "../include/receiver_test/json/json.h"
#define PORT 8080
#define N_CONNECTIONS 10
#define MAX_CONNECTIONS 100
#define MAX_SIZE 1024
std::mutex lock;
std::atomic_bool stop_threads = false;
sem_t x, y;
std::thread tid;
std::thread readwriterthreads[100];
#define LOWER_BOUND 23
#define UPPER_BOUND 30
int readercount = 0;

//References:  GeekforGeeks.org 

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

void process_signal(char* buf, int lower_bound, int upper_bound, std::string name)
{
    // receiving json -----------------------------
    //get size of JSON message
    std::string str(buf);
    size_t pos = str.find("#");
    std::string str_header(str.substr(0, pos));

    //transfer to JSON
    Json::Reader reader;
    Json::Value data;
    bool parsingSuccessful = reader.parse( buf, data );
    if ( !parsingSuccessful )
    {
        std::cout << "Error parsing the string" << std::endl;
    }

    std::cout << "received data: " << data << std::endl;
    // --------------------------------------------
    // Signal processing
    // --------------------------------------------
    std::cout<<"measured signal: "<<data["signal"]<<'\n';
    double signal_value = data["signal"].asDouble();
    if((signal_value >= lower_bound) && (signal_value <= upper_bound))
    {
        std::cout<<name << ": " <<signal_value <<" WITHIN BOUNDS ✅\n";
    }
    else
    {
        std::cout<<name << ": " <<signal_value <<" OUT OF BOUNDS ❌\n";
    }
    // --------------------------------------------    
}

void read_write(int new_socket, struct sockaddr_in address,int delay,std::string name)
{
    while(1){
        char buffer[1024] = { 0 };
        int valread = read(new_socket, buffer, 1024);

        // do stuff
        std::cout<< "received message from address: "<< inet_ntoa(address.sin_addr)<<'\n';
        // process_signal(buffer,LOWER_BOUND,UPPER_BOUND,name);
        puts(buffer);

        // (optional) send message
        // send(new_socket, hello, strlen(hello), 0);

        // closing the connected socket
        close(new_socket);
        if (stop_threads)
        {
            shutdown(new_socket, SHUT_RDWR);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    return;
}

void thread_function(int id,std::string name,int delay)
{
    lock.lock();
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout<<"Init "<<name<<" function: "<<std::ctime(&now_time);
    lock.unlock();

    // ----------------------------------------------------------------
   
    // Initialize variables
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
 
    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);
 
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);
 
    // Bind the socket to the
    // address and port number.
    bind(serverSocket,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));
 
    // Listen on the socket,
    // with 40 max connection
    // requests queued
    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");
 
    // Array for thread
    pthread_t tid[60];
 
    int i = 0;
    
    while(1)
    {
        std::cout << "receiver is running"<<std::endl;
        addr_size = sizeof(serverStorage);
 
        // Extract the first
        // connection in the queue
        newSocket = accept(serverSocket,
                           (struct sockaddr*)&serverStorage,
                           &addr_size); 
        // Creater read/write thread

        readwriterthreads[i++] = std::thread(read_write,newSocket,serverStorage,100, std::to_string(i));
        
        readwriterthreads[i++].join(); 

        if (stop_threads)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    
    }
    return;
}

int main(int argc, char** argv)
{

    std::cout<<"Concurrent program started"<<std::endl;
    std::cout<<"Press \"q\" to end testing"<<std::endl;
    
   

    std::thread th1(thread_function,1,"server", 1000);
    std::thread ip1(input_function,2,"input",10);
    th1.join();
    ip1.join();
    return 0;
}