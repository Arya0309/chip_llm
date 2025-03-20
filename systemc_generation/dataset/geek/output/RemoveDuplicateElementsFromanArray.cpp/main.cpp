
#include <systemc.h>

// Module to remove duplicates from an array
SC_MODULE(RemoveDuplicatesModule) {
    // Input and output ports
    sc_in<sc_uint<32>> clk; // Clock signal
    sc_in<bool> reset;     // Reset signal
    sc_in<sc_uint<32>> data_in; // Input data
    sc_in<bool> valid_in;  // Validity of input data
    sc_out<sc_uint<32>> data_out; // Output data
    sc_out<bool> valid_out; // Validity of output data

    // Internal variables
    sc_uint<32> buffer[10]; // Buffer to hold input data
    int buffer_index;        // Index to track the buffer position
    int write_index;         // Index to track the write position in the output
    bool first_element;      // Flag to check if it's the first element

    // Process to handle data removal of duplicates
    void remove_duplicates() {
        if (reset.read()) {
            // Reset the module state
            buffer_index = 0;
            write_index = 0;
            first_element = true;
        } else if (valid_in.read()) {
            // Read the incoming data
            sc_uint<32> current_data = data_in.read();

            // Check if it's the first element or different from the previous one
            if (first_element || current_data != buffer[buffer_index - 1]) {
                buffer[buffer_index] = current_data;
                buffer_index++;
                first_element = false;
            }

            // Write the output data
            if (buffer_index > 0 && !valid_out.read()) {
                data_out.write(buffer[write_index]);
                valid_out.write(true);
                write_index++;
            }
        } else if (valid_out.read()) {
            // Deassert the output validity
            valid_out.write(false);
        }
    }

    // Constructor to register the process
    SC_CTOR(RemoveDuplicatesModule) {
        SC_METHOD(remove_duplicates);
        sensitive << clk.pos(); // Sensitive to the positive edge of the clock
        dont_initialize(); // Do not initialize the process immediately
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with RemoveDuplicatesModule
    sc_clock clk;          // Clock signal
    sc_signal<bool> reset; // Reset signal
    sc_signal<sc_uint<32>> data_in; // Input data
    sc_signal<bool> valid_in;      // Validity of input data
    sc_signal<sc_uint<32>> data_out; // Output data
    sc_signal<bool> valid_out;     // Validity of output data

    // Instance of RemoveDuplicatesModule
    RemoveDuplicatesModule dup_remover;

    // Process to drive inputs and print outputs
    void drive_and_print() {
        // Reset the module
        reset = true;
        wait(1, SC_NS);
        reset = false;

        // Initialize input data
        sc_uint<32> input_data[] = {1, 2, 2, 3, 4, 4, 4, 5, 5};
        int num_elements = sizeof(input_data) / sizeof(input_data[0]);

        // Drive input data
        for (int i = 0; i < num_elements; i++) {
            data_in = input_data[i];
            valid_in = true;
            wait(1, SC_NS);
            valid_in = false;
            wait(1, SC_NS);
        }

        // Wait for output data
        while (!valid_out.read()) {
            wait(1, SC_NS);
        }

        // Print the results
        cout << "Unique elements: ";
        do {
            cout << data_out.read() << " ";
            wait(1, SC_NS);
        } while (valid_out.read());

        sc_stop(); // Stop simulation
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : clk("clk", 1, SC_NS, 0.5, 0, SC_NS, true) {
        // Connecting signals to the dup_remover ports
        dup_remover.clk(clk);
        dup_remover.reset(reset);
        dup_remover.data_in(data_in);
        dup_remover.valid_in(valid_in);
        dup_remover.data_out(data_out);
        dup_remover.valid_out(valid_out);

        // Registering the drive_and_print process
        SC_THREAD(drive_and_print);
        sensitive << clk.pos(); // Sensitive to the positive edge of the clock
        dont_initialize(); // Do not initialize the process immediately
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
