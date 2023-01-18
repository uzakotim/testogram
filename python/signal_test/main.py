import _thread
from thread_function import thread_function
from checker_function import checker_function
from getch import getch

lock = _thread.allocate_lock()

def main():

    print("Concurrent program started")
    print('Press \"q\" to end testing')
    _thread.start_new_thread( thread_function,("thread", 1,lock) )
    _thread.start_new_thread( checker_function,("checker", 1,lock) )
    while 1:
        c = getch()
        print()
        if c.upper() == 'Q':
            break
        pass
        

if __name__ == "__main__":
    main()
