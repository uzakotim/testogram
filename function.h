// Function
// Develop your function here
/* ----------------------------------------------------- */
std::string findFizzBuzz(int value)
{
    if ((0 == value%3) and (0==value%5))
        return "FizzBuzz";
    if (0 == value%3)
        return "Buzz";
    if (0 == value%5)
        return "Fizz";
    return std::to_string(value);
}
/* ----------------------------------------------------- */
//Copyright Timur Uzakov(c) - 2022