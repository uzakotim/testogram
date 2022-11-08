// Handle Test Function
// Ensure that type of input variable is correct
// Ensure that type of check variable is correct
// and that type of result variable matches check variable
/*----------------------------------------------------------*/
bool err;
#define IS_TRUE(x) {if (!(x)) {\
    std::cerr<<"⛔️ :"  <<" FAILED" <<std::endl;\
    err = true;}}

/*----------------------------------------------------------*/
void handleTest(int test_number, int input, std::string check)
{
    std::string result = findFizzBuzz(input);
    std::cout<<"TEST "<<test_number<<":"<<std::endl;
    IS_TRUE(result==check);
    if (err==false)
        std::cout<<"✅ : PASSED\n";
}
/*----------------------------------------------------------*/
//Copyright Timur Uzakov(c) - 2022