#include "../include/receiver_test/libraries.h"
#include "../include/receiver_test/json/json.h"
#define PORT 8080
#define N_CONNECTIONS 10
#define MAX_CONNECTIONS 100
#define MAX_SIZE 1024
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

void thread_function(int id,std::string name,int delay)
{
    lock.lock();
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout<<"Init "<<name<<" function: "<<std::ctime(&now_time);
    lock.unlock();
    
    double lower_bound = 23.0;
    double upper_bound = 30.0;

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    // char* hello = "Hello from server";
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Set socket as non-blocking

    // where socketfd is the socket you want to make non-blocking
    int status = fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);

    if (status == -1){
    perror("calling fcntl");
    // handle the error.  By the way, I've never seen fcntl fail in this way
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        std::cout << "receiver is running"<<std::endl;
        
        if ((new_socket
            = accept(server_fd, (struct sockaddr*)&address,
                    (socklen_t*)&addrlen))
            >= 0) {
            // perror("accept");
            // exit(EXIT_FAILURE);
            // }
            valread = read(new_socket, buffer, 1024);

            // do stuff
            std::cout<< "received message from address: "<< inet_ntoa(address.sin_addr)<<'\n';
            process_signal(buffer,lower_bound,upper_bound,name);
            // puts(buffer);

            // (optional) send message
            // send(new_socket, hello, strlen(hello), 0);
        
            // closing the connected socket
            close(new_socket);
        }
        if (stop_threads)
        {
            shutdown(server_fd, SHUT_RDWR);
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