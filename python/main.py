# Main entry of the program, which runs all tests
#*----------------------------------------------------------*
# --- import your libraries here ---
#import numpy as np


#*----------------------------------------------------------*
from excuteTest import execute_tests
#*----------------------------------------------------------*
def main():
    err = execute_tests()
    if err == False:
        print("All tests are passed ✅")
    else:
        return

if  __name__ == "__main__":
    main()
#*----------------------------------------------------------*
#Copyright Timur Uzakov(c) - 2022