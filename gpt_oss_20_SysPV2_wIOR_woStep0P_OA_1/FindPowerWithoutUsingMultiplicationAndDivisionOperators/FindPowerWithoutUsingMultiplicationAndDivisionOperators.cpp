#include <cstdint>

int pow(int a, int b)
{
    int result = 1;
    // Static upper bound of 20 (maximum possible value of b)
    for (int i = 0; i < 20; ++i)
    {
        if (i == b) break;   // Stop once the desired exponent is reached
        result *= a;         // Multiply by a each iteration
    }
    return result;
}