
#include <systemc.h>
#include <string>

// Module to remove leading zeros from a string
SC_MODULE(RemoveLeadingZeros) {
    // Input port for the string
    sc_in<std::string> input_str;
    
    // Output port for the processed string
    sc_out<std::string> output_str;

    // Internal storage for the string
    std::string internal_str;

    // Process to remove leading zeros
    void removeZeros() {
        // Read the input string
        internal_str = input_str.read();

        // Count leading zeros
        size_t i = 0;
        while (internal_str[i] == '0')
            i++;

        // Erase leading zeros
        internal_str.erase(0, i);

        // Write the processed string to the output port
        output_str.write(internal_str);
    }

    // Constructor to register the process
    SC_CTOR(RemoveLeadingZeros) {
        SC_METHOD(removeZeros);
        sensitive << input_str;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with RemoveLeadingZeros module
    sc_signal<std::string> input_str;
    sc_signal<std::string> output_str;

    // Instance of RemoveLeadingZeros module
    RemoveLeadingZeros remove_zeros;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the input string
        input_str = "00000123569";

        // Wait for one delta cycle to let the remove_zeros process
        wait(1, SC_NS);

        // Print the results
        cout << "Original string: " << input_str.read() << endl;
        cout << "Processed string: " << output_str.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : remove_zeros("remove_zeros") {
        // Connecting signals to the remove_zeros ports
        remove_zeros.input_str(input_str);
        remove_zeros.output_str(output_str);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
