def checker_function( threadName, delay, lock):
    import time, json
    print (f'Init {threadName}: {time.ctime(time.time())}')
    # Set signal boundaries here
    lower_bound = 23
    upper_bound = 30
   
    while 1:
        try:
            lock.acquire()
            with open('data.json', 'r') as openfile:
                # Reading from json file
                json_object = json.load(openfile)
                signal_value = json_object['signal']
                print("measured signal: " + str(signal_value))    
                
                # Main test of the signal
                if signal_value > lower_bound and signal_value < upper_bound:
                    print(f'{threadName}: {signal_value} WITHIN BOUNDS ✅')
                else:
                    print(f'{threadName}: {signal_value} OUT OF BOUNDS ⛔️')                    
            lock.release()
        except:
            print("cannot open data.json")
        time.sleep(delay)
