
//SystemC code:
#include <systemc.h>
int p = 0;
int n = 153;
temp = n;

// Module that adds two integers

SC_MODULE(ArmstrongChecker) {
    sc_in<int> a;  // Input port for the first number
    sc_in<int> b;  // Input port for the second number
    sc_out<int> rem; // Output port for the remainder

    // Constructor
    SC_CTOR(ArmstrongChecker) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << a << b;
    }

    // Method to add the two numbers
    void add() {
        int rem = a.read() % 10;
        p = (p) + (rem * rem * rem);
        n = n / 10;
    }
};


int main(){
   
    sc_in<int>num_in = 153; // user input
    ArmstrongChecker adder(sc_module_name("adder")); //create a adder module and hook it into the input and output of the system
    sc_out< bool> is_armstrong = adder.read(sc_module_name("adder"), is_armstrong);
 
    

}



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
