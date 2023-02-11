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
    int opt = 1;  
    int master_socket , addrlen , new_socket , client_socket[30] , 
          max_clients = 30 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
         
    char buffer[1025];  //data buffer of 1K 
         
    //set of socket descriptors 
    fd_set readfds;  
         
    //a message 
    char *message = "ECHO Hello world \r\n";  
     
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
         
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
     
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
     
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
         
    //bind the socket to localhost port 8080
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
         
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
         
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
    // ----------------------------------------------------------------
    while(1)
    {
        //clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
             
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
     
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
             
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
             
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
           
            //send new connection greeting message 
            
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            {  
                perror("send");  
            }  
            puts("Welcome message sent successfully");  
                 

            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                         
                    break;  
                }  
            }  
        }  
             
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                         
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    
                    // buffer[valread] = '\0';
                    
                    //do stuff  
                    // process_signal(buffer,LOWER_BOUND,UPPER_BOUND,inet_ntoa(address.sin_addr));
                    puts(buffer);
                }  
            }  
        }
        if(stop_threads)
        {
            for (int i = 0; i <max_clients;i++)
            {
                close(client_socket[i]);
            }
            close(master_socket);
            break;
        }
    }  
    // ----------------------------------------------------------------

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