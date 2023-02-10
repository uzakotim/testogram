import _thread
from getch import getch
import time, json
import socket

stop_threads = False
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#ip = '172.16.231.194'
own_ip = ''
ip = own_ip
port = 8080

# function to receive json packets from client
def receive_json(conn):
    data = conn.recv(1024)
    data = data.decode('utf-8')
    data = json.loads(data)
    return json.loads(data)


def thread_function( threadName, delay):
    global stop_threads
    print (f'Init {threadName}: {time.ctime(time.time())}')
    # set signal boundaries here
    lower_bound = 23
    upper_bound = 30
    # lock = _thread.allocate_lock()
    while 1:
        try:
            connection, client_address = sock.accept()
            print("connection from", client_address)
            # reading from json file
            data = receive_json(connection)
            print("received data:", data)
            signal_value = data['signal']
            
            # main test of the signal
            if signal_value > lower_bound and signal_value < upper_bound:
                print(f'{threadName}: {signal_value} WITHIN BOUNDS ✅')
            else:
                print(f'{threadName}: {signal_value} OUT OF BOUNDS ⛔️')                    
            connection.close()
        except:
            print("problem with receiver")
        if (stop_threads):
            return
        time.sleep(delay)

def main():
    global stop_threads
    print("Concurrent program started")
    print('Press \"q\" to end testing')
    _thread.start_new_thread( thread_function,("thread", 1) )


    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((ip, port))
    print('Socket is listening')
    sock.listen(1)

    while 1:
        c = getch()
        if c.upper() == 'Q':
            sock.close()
            stop_threads = True
        if stop_threads:
            sock.close()
            return
        pass
        

if __name__ == "__main__":
    main()
