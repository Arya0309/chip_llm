#include <iostream>
using namespace std;

bool isPrime(int n) {
    if (n <= 1)
        return false;

    int cnt = 0;
    for (int i = 1; i <= n; i++) {
        if (n % i == 0)
            cnt++;
    }
    return cnt == 2;
}

int main() {
    int n = 29;

    if (isPrime(n))
        cout << n << " is prime" << endl;
    else
        cout << n << " is NOT prime" << endl;

    return 0;
}
