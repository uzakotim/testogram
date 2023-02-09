#include "../include/receiver_test/libraries.h"
#include "../include/receiver_test/json/json.h"
#define PORT 8080
#define N_CONNECTIONS 10
#define MAX_CONNECTIONS 10
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
    struct sockaddr_in address, new_addr;
    int opt = 1;
    int addrlen = sizeof(address);
    int recv_size; // size in bytes received or -1 on error 
    const int size_buf = MAX_SIZE;
    char buf[size_buf] = { 0 };
    fd_set rfds;
    int ret_val,new_fd;
    struct timeval tv;
    /* Wait up to one second. */
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int all_connections[MAX_CONNECTIONS];
    

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int status = fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);
    if (status == -1){
        perror("calling fcntl");
    }
    // printf("Created a socket with fd: %d\n", server_fd);
    
    /* Initialize the socket address structure */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))
        < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd,N_CONNECTIONS) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    for (int i=0;i < MAX_CONNECTIONS;i++) {
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;

    // --------------------------------
    while(1)
    {
        // connecting sockets -------------------------
        FD_ZERO(&rfds);
        for (int i=0;i < MAX_CONNECTIONS;i++) {
             if (all_connections[i] >= 0) {
                 FD_SET(all_connections[i], &rfds);
             }
        }
        ret_val = select(FD_SETSIZE, &rfds, NULL, NULL,&tv);
        
        if (ret_val > 0)
        {
            if(FD_ISSET(server_fd, &rfds))
            {
                new_fd = accept(server_fd, (struct sockaddr*)&new_addr,(socklen_t*)&addrlen);
                if (new_fd >= 0)
                {
                    for (int i=0;i < MAX_CONNECTIONS;i++) {
                         if (all_connections[i] < 0) {
                             all_connections[i] = new_fd; 
                             break;
                         }
                    }
                } else 
                {
                    std::cout << "accept failed\n";
                }
                ret_val--;
                if(!ret_val) continue; 
            }

            /* Check if the fd with event is a non-server fd */
            for (int i=1;i < MAX_CONNECTIONS;i++) {
                if ((all_connections[i] > 0) && (FD_ISSET(all_connections[i], &rfds))) 
                {
                    /* read incoming data */   
                    //  printf("Returned fd is %d [index, i: %d]\n", all_connections[i], i);
                    ret_val = recv(all_connections[i], buf, size_buf, 0);
                    recv_size = sizeof(buf);
                    if (ret_val == 0) {
                    //  printf("Closing connection for fd:%d\n", all_connections[i]);
                        close(all_connections[i]);
                        all_connections[i] = -1; /* Connection is now closed */
                    } 
                    if (ret_val > 0) { 
                    //  printf("Received data (len %d bytes, fd: %d): %s\n", 
                            // ret_val, all_connections[i], buf);
                    } 
                    if (ret_val == -1) {
                    //  printf("recv() failed for fd: %d [%s]\n", 
                            // all_connections[i], strerror(errno));
                        break;
                    }
                }
                ret_val--;
                if (!ret_val) continue;
            }
            // receiving json -----------------------------
            //get size of JSON message
            std::string str(buf);
            size_t pos = str.find("#");
            std::string str_header(str.substr(0, pos));

            // sanity check
            buf[recv_size - 1] = '\0';
            assert(str_header.compare(buf) == 0);

            //transfer to JSON
            Json::Reader reader;
            Json::Value data;
            bool parsingSuccessful = reader.parse( buf, data );
            if ( !parsingSuccessful )
            {
                std::cout << "Error parsing the string" << std::endl;
            }



            // --------------------------------------------
            // Signal processing
            // --------------------------------------------
            std::cout<<"Measured signal: "<<data["signal"]<<'\n';
            double signal_value = data["signal"].asDouble();
            if((signal_value >= lower_bound) && (signal_value <= upper_bound))
            {
                std::cout<<"WITHIN BOUNDS ✅\n";
            }
            else
            {
                std::cout<<"OUT OF BOUNDS ❌\n";
            }
            // --------------------------------------------
        }
        else 
        {
            std::cout<<"waiting for signal"<<std::endl;
        }

        if (stop_threads){
            for (int i=0;i < MAX_CONNECTIONS;i++) {
                if (all_connections[i] > 0) {
                    close(all_connections[i]);
                }
            }
            close(server_fd);
            return;
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