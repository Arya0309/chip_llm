
   #include <systemc.h>

   
SC_MODULE(PrimeChecker) {
       sc_in<int> n;
       sc_out<bool> is_prime;

       void check_prime() {
           int cnt = 0;

           // If number is less than/equal to 1,
           // it is not prime
           if (n.read() <= 1)
               is_prime.write(false);
           else {
               // Check for divisors from 1 to n
               for (int i = 1; i <= n.read(); i++) {
                   // Check how many number is divisible
                   // by n
                   if (n.read() % i == 0)
                       cnt++;
               }

               // If n is divisible by more than 2 numbers
               // then it is not prime
               if (cnt > 2)
                   is_prime.write(false);
               // else it is prime
               else
                   is_prime.write(true);
           }
       }

       SC_CTOR(PrimeChecker) {
           SC_METHOD(check_prime);
           sensitive << n;
       }
   };

   

SC_MODULE(Testbench) {
    sc_signal<int> n_sig;       // Signal for the input number
    sc_signal<bool> prime_sig;  // Signal for the prime check result

    // Instance of the PrimeChecker module
    PrimeChecker prime_checker_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : prime_checker_inst("prime_checker_inst") {
        // Connect signals to module ports
        prime_checker_inst.n(n_sig);
        prime_checker_inst.is_prime(prime_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 29 (expected: prime)
        n_sig.write(29);
        wait(1, SC_NS); // Wait for the process to update
        bool result = prime_sig.read();
        std::cout << "Number " << n_sig.read() << " is " << (result ? "prime" : "NOT prime") << std::endl;
        assert(result == true);

        // Test case: n = 1 (expected: NOT prime)
        n_sig.write(1);
        wait(1, SC_NS);
        result = prime_sig.read();
        std::cout << "Number " << n_sig.read() << " is " << (result ? "prime" : "NOT prime") << std::endl;
        assert(result == false);

        // Test case: n = 10 (expected: NOT prime)
        n_sig.write(10);
        wait(1, SC_NS);
        result = prime_sig.read();
        std::cout << "Number " << n_sig.read() << " is " << (result ? "prime" : "NOT prime") << std::endl;
        assert(result == false);

        // Test case: n = 13 (expected: prime)
        n_sig.write(13);
        wait(1, SC_NS);
        result = prime_sig.read();
        std::cout << "Number " << n_sig.read() << " is " << (result ? "prime" : "NOT prime") << std::endl;
        assert(result == true);

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
