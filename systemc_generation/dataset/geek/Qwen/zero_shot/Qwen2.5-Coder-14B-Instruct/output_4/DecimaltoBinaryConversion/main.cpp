
#include <systemc.h>


SC_MODULE(DecToBinary) {
    sc_in<int> dec_in; // Input signal for decimal number
    sc_out<int> binary_out; // Output signal for binary number

    SC_CTOR(DecToBinary) {
        SC_METHOD(convert);
        sensitive << dec_in;
    }

    void convert() {
        int n = dec_in.read();
        int binaryNum[32];
        int i = 0;

        while (n > 0) {
            binaryNum[i] = n % 2;
            n = n / 2;
            i++;
        }

        // Convert binary array to a single integer
        int binary_result = 0;
        for (int j = 0; j < i; j++) {
            binary_result = binary_result * 2 + binaryNum[j];
        }

        binary_out.write(binary_result);
    }
};



SC_MODULE(Testbench) {
    sc_signal<int> dec_sig;    // Signal for the decimal input
    sc_signal<int> bin_sig;    // Signal for the binary output

    DecToBinary* dec_to_binary_inst;  // Instance of the conversion module

    SC_CTOR(Testbench) {
        // Instantiate the DecToBinary module and connect signals
        dec_to_binary_inst = new DecToBinary("DecToBinary");
        dec_to_binary_inst->dec_in(dec_sig);
        dec_to_binary_inst->binary_out(bin_sig);

        SC_THREAD(run_tests);   // Launch test cases in a thread
    }

    void run_tests() {
        // Test Case: Input n = 10 should result in binary 1010
        dec_sig.write(10);
        wait(1, SC_NS);  // Wait for the process to trigger

        int result = bin_sig.read();
        std::cout << "Input: 10, Binary Output: " << result << std::endl;
        assert(result == 1010);

        // Test Case: Input n = 0 should result in binary 0
        dec_sig.write(0);
        wait(1, SC_NS);  // Wait for the process to trigger

        int result2 = bin_sig.read();
        std::cout << "Input: 0, Binary Output: " << result2 << std::endl;
        assert(result2 == 0);

        // Test Case: Input n = 1000 should result in binary 1111101000
        dec_sig.write(1000);
        wait(1, SC_NS);  // Wait for the process to trigger
        int result3 = bin_sig.read();
        std::cout << "Input: 1000, Binary Output: " << result3 << std::endl;
        assert(result3 == 1111101000);

        // Print success message and stop simulation
        std::cout << "Test passed successfully." << std::endl;
        sc_stop();
    }

    // Destructor to free allocated memory
    ~Testbench() {
        delete dec_to_binary_inst;
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");  // Create an instance of the Testbench module
    sc_start();          // Start the simulation
    return 0;
}
