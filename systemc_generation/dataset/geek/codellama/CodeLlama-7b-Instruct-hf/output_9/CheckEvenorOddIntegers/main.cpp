scss
#include <systemc.h>

// Define input and output signals
sc_in<int> number_in;  // Input integer
sc_out<string> result_out;  // Output string

// Define a module to check if a number is even or odd

SC_MODULE(EvenOddChecker) {
    // Declare input and output ports
    sc_in<int> number;
    sc_out<string> result;

    // Define a process to check if the number is even or odd
    void check_even_or_odd() {
        // Conditionally assign "Even" or "Odd" to result based on the value of number
        if (number % 2 == 0)
            result = "Even";
        else
            result = "Odd";
    }

    // Constructor
    SC_CTOR(EvenOddChecker) {
        // Call check_even_or_odd() method
        SC_METHOD(check_even_or_odd);
        sensitive << number;
    }
};

// Define a driver module to generate input and output testbench signals

SC_MODULE(driver) {
    // Declare input and output ports
    sc_out<int> number_out;
    sc_out<string> result_out;

    // Define a process to generate random input and output testbench signals
    void generate_random_inputs() {
        // Generate random integer input between 0 and 100
        int random_number = rand() % 100;
        number_out.write(random_number);
    }

    // Define a process to check the output of the EvenOddChecker module
    void check_output() {
        // Read the output of the EvenOddChecker module
        string result = result_out.read();
        // Check if the result is "Even" or "Odd"
        if (result == "Even" || result == "Odd")
            cout << "Result: " << result << endl;
        else
            cerr << "Result: " << result << endl;
    }

    // Constructor
    SC_CTOR(driver) {
        // Call generate_random_inputs() and check_output() methods
        SC_METHOD(generate_random_inputs);
        SC_METHOD(check_output);
        // Sensitivity list for generate_random_inputs() method
        sensitive << number_out;
        // Sensitivity list for check_output() method
        sensitive << result_out;
    }
};

// Define a top-level module that instantiates the EvenOddChecker and driver modules

SC_MODULE(top_level) {
    // Instantiate the EvenOddChecker and driver modules
    EvenOddChecker even_odd;
    driver driver;

    // Constructor
    SC_CTOR(top_level) {
        // Add sensitivity to input and output signals
        sensitive << number_in << result_out;
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> number_sig;   // Signal for the input number
    sc_signal<int> result_sig;   // Signal for the result

    EvenOddChecker checker_inst; // Instance of the EvenOddChecker module

    SC_CTOR(Testbench)
    : checker_inst("checker_inst")
    {
        checker_inst.number_in(number_sig);
        checker_inst.result_out(result_sig);
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Test case 1: Number 11 (Odd)
        number_sig.write(11);
        wait(1, SC_NS);
        std::cout << result_sig.read() << std::endl; // Expected output: 1
        assert(result_sig.read() == 1);

        // Test case 2: Number 12 (Even)
        number_sig.write(12);
        wait(1, SC_NS);
        std::cout << result_sig.read() << std::endl; // Expected output: 0
        assert(result_sig.read() == 0);
        
        std::cout << "All tests pass successfully!" << std::endl;
        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
