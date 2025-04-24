scss
// Include the necessary headers
#include <systemc.h>

// Define the inputs and outputs
sc_in<int> n;
sc_out<int> sum;


SC_MODULE(FibonacciSumCalculator) {
   // Declare the input and output ports
   sc_in<int> n;
   sc_out<int> sum;

   // Define the Fibonacci sequence
   int fibo[2 * (n + 1)]; // Array of size 2 \* (n + 1)
   fibo[0] = 0, fibo[1] = 1; // Initialize the first two elements

   // Define the sum variable
   int sum;

   // Define the add function
   void add(int, int, int);

   // Define the main function
   SC_CTOR(FibonacciSumCalculator) {
      // Call the add function
      add(0, 1, n);
   }

   // Function to compute the sum
   void add(int i, int a, int b) {
      // Check if the input is less than or equal to 0
      if (b <= 0) {
         return 0;
      }

      // Compute the value of Fibonacci numbers
      fibo[i] = fibo[i - 1] + fibo[i - 2];

      // Check if the current index is even
      if (i % 2 == 0) {
         sum += fibo[i];
      }

      // Recursively call the add function
      add(i + 1, fibo[i], b - 1);
   }
};


SC_MODULE(Testbench) {
    sc_signal<int> n_sig; // Signal for the number of terms
    sc_signal<int> sum_sig; // Signal for the sum of even-indexed Fibonacci numbers

    // Instance of the FibonacciSumCalculator module
    FibonacciSumCalculator fib_sum_calc_inst;

    // Constructor
    SC_CTOR(Testbench)
    : fib_sum_calc_inst("fib_sum_calc_inst") {
        // Connect signals to module ports
        fib_sum_calc_inst.n(n_sig);
        fib_sum_calc_inst.sum(sum_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 8 (expected sum = 44)
        n_sig.write(8);
        wait(1, SC_NS); // Wait for the process to update
        int result = sum_sig.read();
        std::cout << "Even indexed Fibonacci Sum upto " << n_sig.read() << " terms: " << result << std::endl;
        assert(result == 1596);

        // Additional test case: n = 5 (expected sum = 10)
        n_sig.write(5);
        wait(1, SC_NS);
        result = sum_sig.read();
        std::cout << "Even indexed Fibonacci Sum upto " << n_sig.read() << " terms: " << result << std::endl;
        assert(result == 88);

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
