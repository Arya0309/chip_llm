// C Program to Find Prime Numbers in a Given Range using Trial Division method
#include <stdbool.h>
#include <stdio.h>

bool isPrime(int n) {

    // Checking primality by finding a complete division
      // in the range 2 to n-1
    if (n <= 1)
        return false;
    for (int i = 2; i < n; i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

void findPrimes(int l, int r) {

    // Flag to check if any prime numbers are found
    bool found = false;
    for (int i = l; i <= r; i++) {

        // Checking if the number is prime
        if (isPrime(i)) {
            printf("%d ", i);
            found = true;
        }
    }
    if (!found) {
        printf(
            "No prime numbers found in the given range.");
    }
}

int main() {
    int l = 10, r = 30;
    
      // Finding and printing the prime between [l, r]
    findPrimes(l, r);
  
    return 0;
}
