# Handle Test Function
# *----------------------------------------------------------*
from function import functionName
# *----------------------------------------------------------*
def handleTest(test_number, input, check):
    result = functionName(input)
    print("TEST ",test_number,":",sep='')
    if (result!=check):
        print("⛔️: ","FAILED",sep='')
        error = True
    else:
        print("✅: PASSED")
        error = False
    return error
# *----------------------------------------------------------*