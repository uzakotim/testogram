import logging
import _thread
import time
import json
import sys
from readchar import readkey, key

lock = _thread.allocate_lock()

def thread_function( threadName, delay):
    print (f'Init {threadName}: {time.ctime(time.time())}')
    i = 20
    while 1:
        print(f'{threadName}: running')
        # temperature sensor
        i += 1
        if (i > 25):
            i = 20
        dictionary = {'signal':i}
        # Serializing json
        json_object = json.dumps(dictionary, indent=1)
        # Writing to sample.json
        lock.acquire()
        with open("data.json", "w") as outfile:
            outfile.write(json_object)
        lock.release()
        time.sleep(delay)

def checker_function( threadName, delay):
    print (f'Init {threadName}: {time.ctime(time.time())}')
    lower_bound = 22
    upper_bound = 30
    while 1:
        try:
            lock.acquire()
            with open('data.json', 'r') as openfile:
                # Reading from json file
                json_object = json.load(openfile)
                signal_value = json_object['signal']
                print("measured signal: " + str(signal_value))
                if signal_value > lower_bound and signal_value < upper_bound:
                    print(f'{threadName}: {signal_value} WITHIN BOUNDS ✅')
                else:
                    print(f'{threadName}: {signal_value} OUT OF BOUNDS ⛔️')
            lock.release()
        except:
            print("cannot open data.json")
        time.sleep(delay)



def main():

    print("Concurrent program started")
    _thread.start_new_thread( thread_function,("thread", 1) )
    _thread.start_new_thread( checker_function,("checker", 1) )
    while 1:
        k = readkey()
        if k == key.q:
            _thread.exit
        else:
            pass
        

if __name__ == "__main__":
    main()
