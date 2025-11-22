#include <iostream>
using namespace std;

int findLCM(int a, int b) {
    int max_num = (a > b) ? a : b;

    while (true) {
        if (max_num % a == 0 && max_num % b == 0) {
            return max_num;
        }
        ++max_num;
    }
}

int main() {
    int a = 15, b = 20;
    int lcm = findLCM(a, b);
    cout << "LCM of " << a << " and " << b << " is " << lcm;
    return 0;
}
