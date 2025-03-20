
#include <systemc.h>
#include <string>

// Module to remove leading zeros from a string
SC_MODULE(RemoveLeadingZeros) {
    sc_in<std::string> input_str;  // Input port for the string
    sc_out<std::string> output_str; // Output port for the modified string

    // Constructor
    SC_CTOR(RemoveLeadingZeros) {
        // Process to remove leading zeros
        SC_METHOD(remove_zeros);
        sensitive << input_str;
    }

    // Method to remove leading zeros
    void remove_zeros() {
        std::string str = input_str.read();
        int i = 0;
        while (str[i] == '0') {
            i++;
        }
        str.erase(0, i);
        output_str.write(str);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<std::string> input_str;  // Signal for the input string
    sc_signal<std::string> output_str; // Signal for the output string

    RemoveLeadingZeros rlz_inst; // Instance of the RemoveLeadingZeros module

    // Constructor
    SC_CTOR(Testbench) : rlz_inst("rlz_inst") {
        // Connect signals to ports
        rlz_inst.input_str(input_str);
        rlz_inst.output_str(output_str);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1
        input_str.write("00000123569");
        wait(1, SC_NS); // Wait for the module to process
        std::cout << "Modified String: " << output_str.read() << std::endl;
        assert(output_str.read() == "123569");

        // Test case 2
        input_str.write("00000000000");
        wait(1, SC_NS);
        std::cout << "Modified String: " << output_str.read() << std::endl;
        assert(output_str.read() == "");

        // Test case 3
        input_str.write("123456789");
        wait(1, SC_NS);
        std::cout << "Modified String: " << output_str.read() << std::endl;
        assert(output_str.read() == "123456789");

        // Test case 4
        input_str.write("000000000001");
        wait(1, SC_NS);
        std::cout << "Modified String: " << output_str.read() << std::endl;
        assert(output_str.read() == "1");

        // Print success message
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
