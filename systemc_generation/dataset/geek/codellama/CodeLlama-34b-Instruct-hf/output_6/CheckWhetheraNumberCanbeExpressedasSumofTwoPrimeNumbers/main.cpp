
#include <systemc.h>
using namespace sc_core;


SC_MODULE(PrimeSumChecker) {
  sc_in<int> num;
  sc_out<bool> is_prime;

  void isPrime() {
    for (int i = 2; i < num; i++) {
      if (num % i == 0) {
        is_prime.write( false);
        return;
      }
    }
    is_prime.write(true);
  }
};


SC_MODULE(IsSumOfPrimes) {
  sc_in<int> n;
  sc_out<bool> is_sum_of_primes;
  
  PrimeSumChecker prime0, prime1;
  sc_signal<int> num0, num1;

  void isSumOfPrimes() {
    num0 = n - 2;
    num1 = n;
    prime0.num = num0;
    prime1.num = num1;
    prime0.isPrime();
    prime1.isPrime();
    if (prime0.is_prime.read() && prime1.is_prime.read()) {
      is_sum_of_primes.write(true);
    } else {
      is_sum_of_primes.write(false);
    }
  }
};



SC_MODULE(Testbench) {
    sc_signal<int> n_sig;       // Signal for the input number
    sc_signal<bool> result_sig; // Signal for the result

    // Instance of the PrimeSumChecker module
    PrimeSumChecker checker_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : checker_inst("checker_inst") {
        // Connect signals to module ports
        checker_inst.n_in(n_sig);
        checker_inst.result_out(result_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 13 (expected result: true)
        n_sig.write(13);
        wait(1, SC_NS); // Wait for the process to update
        bool result = result_sig.read();
        std::cout << "Can 13 be expressed as the sum of two primes? " << (result ? "Yes" : "No") << std::endl;
        assert(result == true);

        // Additional test case: n = 17 (expected result: false)
        n_sig.write(17);
        wait(1, SC_NS);
        result = result_sig.read();
        std::cout << "Can 17 be expressed as the sum of two primes? " << (result ? "Yes" : "No") << std::endl;
        assert(result == false);

        // End simulation after successful tests
        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
