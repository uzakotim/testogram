def thread_function( threadName, delay, lock):
    import time
    import json
    print (f'Init {threadName}: {time.ctime(time.time())}')
    i = 20
    while 1:
        print(f'{threadName}: running')
        # temperature sensor simulation
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
        
        # Delay time
        time.sleep(delay)