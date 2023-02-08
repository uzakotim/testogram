import _thread
from getch import getch
import time,json
import socket

# ip = '172.16.176.137'
ip_to_send = '172.16.231.194'
ip = ip_to_send
port = 8080
stopThreads = False

# send json packet to server
def send_json_packet(sock, json_packet):
    sock.send(json.dumps(json_packet).encode())


def thread_function( threadName, delay):
    global stopThreads
    print (f'Init {threadName}: {time.ctime(time.time())}')

    lock = _thread.allocate_lock()
    i = 20
    while 1:
        print(f'{threadName}: running')
        # ------------------------------------
        # temperature sensor simulation
        # replace with your signal processing code
        i += 1
        if (i > 25):
            i = 20
        dictionary = {'signal':i} 
        # ------------------------------------
        # serializing json
        json_object = json.dumps(dictionary, indent=1)
        
        # writing to data.json
        lock.acquire()
        with open("data.json", "w") as outfile:
            outfile.write(json_object)
        lock.release()
        # sending
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:    
            sock.connect((ip,int(port)))
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            send_json_packet(sock,json_object)        
            sock.close()
        except:
            pass
        if stopThreads:
            return
        # delay time
        time.sleep(delay)


def main():
    global stopThreads
    print("Concurrent send network program started")
    print('Press \"q\" to end testing')
    _thread.start_new_thread( thread_function,("sender", 1) )
    while 1:
        c = getch()
        print()
        if c.upper() == 'Q':
            stopThreads = True
        pass
        if stopThreads:
            return 
        

if __name__ == "__main__":
    main()



