// Synthesizable version: no I/O, pure functions only

const int MAX_N = 10000; // Static upper bound for loops

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i <= MAX_N; ++i) {
        if (i * i > n) break;          // Stop when i^2 exceeds n
        if (n % i == 0) return false;  // Divisor found
    }
    return true;
}

bool isPossible(int n) {
    for (int i = 2; i <= MAX_N; ++i) {
        if (i > n / 2) break;          // No need to check beyond n/2
        if (isPrime(i) && isPrime(n - i)) return true;
    }
    return false;
}

// Top-level function that can be called from a testbench
int solve(int n) {
    return isPossible(n) ? 1 : 0;
}