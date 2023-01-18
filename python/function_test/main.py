# Main entry of the program, which runs all tests
#*----------------------------------------------------------*
# --- import your libraries here ---
#import numpy as np


#*----------------------------------------------------------*
from executeTest import execute_tests
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