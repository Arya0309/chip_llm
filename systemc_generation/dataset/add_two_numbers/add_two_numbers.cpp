#include <iostream>
#include <cassert>

// Function that adds two natural numbers
unsigned int add(unsigned int a, unsigned int b) {
    return a + b;
}

// Testbench for the add function
void test_add() {
    // Test case 1: Adding zero values
    assert(add(0, 0) == 0);
    // Test case 2: Simple addition
    assert(add(1, 2) == 3);
    // Test case 3: Larger numbers
    assert(add(100, 200) == 300);
    // Additional test case: One operand is zero
    assert(add(0, 5) == 5);
    
    std::cout << "All tests passed successfully.\n";
}

int main() {
    // Run the testbench to validate the add function
    test_add();

    // Regular usage: read two natural numbers from the user and display their sum
    unsigned int num1, num2;
    std::cout << "Enter two natural numbers: ";
    std::cin >> num1 >> num2;
    
    std::cout << "Sum: " << add(num1, num2) << std::endl;
    
    return 0;
}
