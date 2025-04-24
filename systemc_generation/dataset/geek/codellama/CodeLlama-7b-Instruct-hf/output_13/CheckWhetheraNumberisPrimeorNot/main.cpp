
#include <systemc.h>

// Summary
// This C++ program to verify whether a number is a prime number using
// simple trial division. It will output whether n is a prime number
//  or not.


SC_MODULE(PrimeChecker){

// Declare inputs
// Input for n
sc_in<int>  n;

// Output for whether n is prime.
sc_out<bool> is_prime;

// Constructor
// Initialize a boolean variable to keep track of whether n is a prime
// number or not.
// Initialize cnt to 0. Loop through i values from 1 to the value of n then
// if cnt is less than 2 after the loop, that means n only divided into 1 or
// itself so it's a prime number. If it divided into more than that, then it
//is not a prime number.
SC_CTOR(PrimeChecker){
    cnt = 0;
SC_METHOD(PrimeChecker);
sensitive << n;
}

// PrimeChecker Method
// Iterate through n's divisors
// Count how many divisors n has
// Check if n is a prime number
void PrimeChecker(){
  
int cnt = 0;
for (int i = 1; i <= n; i++){
    if (n % i == 0){
        cnt++;
    }
}
    if (cnt> 2){
        cout << "is NOT prime" << endl;
    }
    else{
        cout << "is prime" << endl;
    }
}

// Return the value of is_prime
is_prime.write(cnt>2);

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
