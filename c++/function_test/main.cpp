// Main entry of the program, which runs all tests
/*----------------------------------------------------------*/
#include "libraries.h"
#include "function.h"
#include "executeTest.h"
/*----------------------------------------------------------*/
int main(void)
{ 
    execute_tests();
    if (err == false) std::cout<<"All tests are passed\n";
}
/*----------------------------------------------------------*/