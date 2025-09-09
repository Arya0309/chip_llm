// Synthesis‑ready factorial function for n < 20
constexpr unsigned int factorial_table[20] = {
    1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880,
    3628800, 39916800, 479001600, 6227020800, 87178291200,
    1307674368000, 20922789888000, 355687428096000,
    6402373705728000, 121645100408832000
};

unsigned int factorial(unsigned int n)
{
    // Assume n < 20; synthesis tools can verify this constraint
    return factorial_table[n];
}