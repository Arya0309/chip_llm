#include <cassert>
#include <iostream>

// Define a struct to represent a complex number.
struct Complex {
    double real;
    double imag;
};

// Function to add two complex numbers.
Complex add(const Complex &a, const Complex &b) {
    return {a.real + b.real, a.imag + b.imag};
}

// Test bench function.
void test_add() {
    // Test Case 1: Basic addition.
    Complex c1{1.0, 2.0};
    Complex c2{3.0, 4.0};
    Complex result = add(c1, c2);
    assert(result.real == 4.0);
    assert(result.imag == 6.0);

    // Test Case 2: Adding zero.
    Complex zero{0.0, 0.0};
    Complex result2 = add(c1, zero);
    assert(result2.real == c1.real);
    assert(result2.imag == c1.imag);

    // Test Case 3: Adding negative numbers.
    Complex negative{-1.0, -2.0};
    Complex result3 = add(c1, negative);
    assert(result3.real == 0.0);
    assert(result3.imag == 0.0);
}

int main() {
    test_add();
    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
