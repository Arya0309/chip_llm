// Synthesizable LCM calculation without I/O or unbounded loops
int gcd(int a, int b) {
    // Euclidean algorithm – bounded by log(min(a,b))
    while (b != 0) {
        int temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

int lcm(int a, int b) {
    // Handle zero inputs to avoid division by zero
    if (a == 0 || b == 0) {
        return 0;
    }
    int divisor = gcd(a, b);
    return (a / divisor) * b;
}