scss
#include <systemc.h>

// Function to calculate the sum of the cubes of the digits of a given number
void calculate_armstrong_number(int n) {
  // Declare and initialize variables
  int temp = 0;
  int p = 0;

  // Iterate until n is 0
  while (n > 0) {
    // Take the remainder of n divided by 10 and multiply the result by the result squared
    p = (p) + (n % 10 * (n % 10) * (n % 10));

    // Divide p by 10
    p = p / 10;

    // Divide n by 10
    n = n / 10;
  }

  // Check if temp is equal to p
  if (temp == p) {
    // Set is_armstrong to true
    is_armstrong.write(true);
  } else {
    // Set is_armstrong to false
    is_armstrong.write(false);
  }
}

// Print is_armstrong
sc_out<bool> is_armstrong;

// Main module

SC_MODULE(ArmstrongChecker) {
  SC_HAS_PROCESS(ArmstrongChecker);

  // Module constructor
  ArmstrongChecker(sc_module_name name) : sc_module(name) {
    SC_CTHREAD(calculate_armstrong_number, clock);
    reset_signal_is(reset, true);
  }

  // Function to calculate the sum of the cubes of the digits of a given number
  void calculate_armstrong_number() {
    // Read input num_in
    sc_in<int> num_in = sc_read();

    // Initialize variables
    int n = num_in.read();
    int temp = 0;
    int p = 0;

    // Call calculate_armstrong_number function
    calculate_armstrong_number(n);

    // Print is_armstrong
    sc_out<bool> is_armstrong = sc_read();
    cout << is_armstrong << endl;
  }
};


SC_MODULE(Testbench) {
    sc_signal<int> num_sig;       // Signal for the number input
    sc_signal<bool> armstrong_sig; // Signal for the Armstrong check result

    // Instance of the ArmstrongChecker module
    ArmstrongChecker armstrong_checker_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : armstrong_checker_inst("armstrong_checker_inst") {
        // Connect signals to module ports
        armstrong_checker_inst.num_in(num_sig);
        armstrong_checker_inst.is_armstrong(armstrong_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 153 (expected: Armstrong number)
        num_sig.write(153);
        wait(1, SC_NS); // Wait for the process to update
        bool result = armstrong_sig.read();
        std::cout << "Number: 153 is Armstrong? " << (result ? "Yes" : "No") << std::endl;
        assert(result == true);

        // Test case: n = 123 (expected: Not an Armstrong number)
        num_sig.write(123);
        wait(1, SC_NS); // Wait for the process to update
        result = armstrong_sig.read();
        std::cout << "Number: 123 is Armstrong? " << (result ? "Yes" : "No") << std::endl;
        assert(result == false);

        // End simulation after successful test
        std::cout << "Tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
