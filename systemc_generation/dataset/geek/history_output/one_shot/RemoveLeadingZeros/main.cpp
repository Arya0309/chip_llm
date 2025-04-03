
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
        
        // Count leading zeros
        int i = 0;
        while (str[i] == '0')
            i++;

        // Erase leading zeros
        str.erase(0, i);

        // Write the modified string to the output port
        output_str.write(str);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<std::string> input_str; // Signal for the input string
    sc_signal<std::string> output_str; // Signal for the output string

    RemoveLeadingZeros rlz_inst; // Instance of the RemoveLeadingZeros module

    // Constructor
    SC_CTOR(Testbench) : rlz_inst("rlz") {
        // Connect signals to ports
        rlz_inst.input_str(input_str);
        rlz_inst.output_str(output_str);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: String with leading zeros
        input_str = "00000123569";
        wait(1, SC_NS); // Wait for the module to process
        assert(output_str.read() == "123569");
        cout << "Test case 1 passed: " << output_str.read() << endl;

        // Test case 2: String without leading zeros
        input_str = "123456";
        wait(1, SC_NS);
        assert(output_str.read() == "123456");
        cout << "Test case 2 passed: " << output_str.read() << endl;

        // Test case 3: String with all zeros
        input_str = "000000";
        wait(1, SC_NS);
        assert(output_str.read() == "");
        cout << "Test case 3 passed: " << output_str.read() << endl;

        // Test case 4: Empty string
        input_str = "";
        wait(1, SC_NS);
        assert(output_str.read() == "");
        cout << "Test case 4 passed: " << output_str.read() << endl;

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
