# Function to write commands to produce tests
# Write your tests here
#* ----------------------------------------------------- *
from handleTest import handleTest
def execute_tests():
    # handleTest(testNumber, input, reference)
    # ---------------------------------------
    error = handleTest(1,1, "1")
    if error == True:
        return True
    # ---------------------------------------
    # ---------------------------------------
    error = handleTest(2,3, "Buzz")
    if error == True:
        return True
    # ---------------------------------------
    # ---------------------------------------
    error = handleTest(3,5, "Fizz")
    if error == True:
        return True
    # ---------------------------------------
    # ---------------------------------------
    error = handleTest(4,15, "FizzBuzz")
    if error == True:
        return True
    # ---------------------------------------
    return False
#* ----------------------------------------------------- *
