/* === IO REQUIREMENT (HINT) ===
Expected input consists of:
   a number n, where n<1000
Expected output consists of:
    a number
*/
#include <array>
using namespace std;

constexpr std::array<int, 1001> build_fib_table() {
    std::array<int, 1001> a{};
    a[0] = 0;
    a[1] = 1;
    for (int i = 2; i <= 1000; ++i) {
        a[i] = a[i - 1] + a[i - 2];
    }
    return a;
}

constexpr std::array<int, 1001> fib_table = build_fib_table();

int fib(int n) {
    return fib_table[n];
}