
#include <systemc.h>

// Module to check if a number is even or odd
SC_MODULE(EvenOddChecker) {
    sc_in<int> number;  // Input port for the number to check
    sc_out<bool> is_even; // Output port indicating if the number is even
    sc_out<bool> is_odd;  // Output port indicating if the number is odd

    // Constructor
    SC_CTOR(EvenOddChecker) {
        // Process to check if the number is even or odd
        SC_METHOD(check_even_odd);
        sensitive << number;
    }

    // Method to check if the number is even or odd
    void check_even_odd() {
        bool even = (number.read() % 2 == 0);
        bool odd = !even;
        is_even.write(even);
        is_odd.write(odd);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> number; // Signal for the number to check
    sc_signal<bool> is_even; // Signal for the even check result
    sc_signal<bool> is_odd;  // Signal for the odd check result

    EvenOddChecker even_odd_checker_inst; // Instance of the EvenOddChecker module

    // Constructor
    SC_CTOR(Testbench) : even_odd_checker_inst("even_odd_checker") {
        // Connect signals to ports
        even_odd_checker_inst.number(number);
        even_odd_checker_inst.is_even(is_even);
        even_odd_checker_inst.is_odd(is_odd);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Check if 11 is odd
        number = 11;
        wait(1, SC_NS); // Wait for the EvenOddChecker to process
        assert(is_even.read() == false);
        assert(is_odd.read() == true);
        cout << "Number: " << number.read() << " is Odd" << endl;

        // Test case 2: Check if 12 is even
        number = 12;
        wait(1, SC_NS);
        assert(is_even.read() == true);
        assert(is_odd.read() == false);
        cout << "Number: " << number.read() << " is Even" << endl;

        // Test case 3: Check if 0 is even
        number = 0;
        wait(1, SC_NS);
        assert(is_even.read() == true);
        assert(is_odd.read() == false);
        cout << "Number: " << number.read() << " is Even" << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
