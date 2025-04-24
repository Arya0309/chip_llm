CPP
// C++ Program to find sum of even-indiced Fibonacci numbers
#include <systemc.h>
#include <bits/stdc++.h>
using namespace std;

// Computes value of first fibonacci numbers and 
//  stores the even-indexed sum
int FibonacciSumCalculator(int n)
{
	if (n <= 0)
		return 0;

	int fibo[2 * n + 1];
	fibo[0] = 0, fibo[1] = 1;

	// Initialize result
	int sum = 0;

	// Add remaining terms
	for (int i = 2; i <= 2 * n; i++) 
	{
		fibo[i] = fibo[i - 1] + fibo[i - 2];

		// For even indices
		if (i % 2 == 0)
			sum += fibo[i];
	}

	// Return the alternating sum
	return sum;
}


// SystemC Module that drives the calculation

SC_MODULE(FibonacciSumCalculator) {
    sc_in<int> n;
    sc_out<int> sum;

    // Constructor
    SC_CTOR(FibonacciSumCalculator) {
    	sc_in<int> fibo_0 = 0;
    	sc_in<int> fibo_1 = 1;
    	sc_out<int> fibo_2;
    	sc_out<int> fibo_3;
    	sc_out<int> fibo_4;
    	
        // Call FibonacciSumCalculator function
        SC_METHOD(FibonacciSumCalculator);
        sensitive << n << sum; 
     }

     // Method to find the sum of even-indiced fibonacci numbers

     int FibonacciSumCalculator() {

        // Compute the sum of the even-indiced fibonacci numbers
        int sum;
        for (int i = 2; i <= n; i++)
        {
                fibo.push_back(fibo[i - 1] + fibo[i - 2]);
                if (i % 2 == 0)
                {
                        sum += fibo[i];
                }
        }

        // Return the sum
        return sum;
     }

}

// Driver code
int main()
{

	// Get n
	int n = 8;

	// Find the even-indiced sum
	 cout << "Even indexed Fibonacci Sum upto " << n << " terms: " << FibonacciSumCalculator(n) << endl;

	return 0;
}


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
