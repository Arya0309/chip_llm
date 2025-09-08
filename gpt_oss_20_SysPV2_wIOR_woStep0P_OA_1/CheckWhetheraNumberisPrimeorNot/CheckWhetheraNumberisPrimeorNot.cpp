#include <cstdint>

constexpr int MAX_N = 1000;  // Maximum value considered for the loop

// Returns 1 if n is prime, 0 otherwise
int is_prime_flag(int n) {
    if (n <= 1) return 0;          // 0 and 1 are not prime

    for (int i = 2; i < MAX_N && i < n; ++i) {
        if (n % i == 0) return 0; // Divisor found
    }
    return 1;                     // No divisor found
}

// Top‑level function that can be synthesized as a hardware module
void prime_checker(int n, int &prime) {
    prime = is_prime_flag(n);
}