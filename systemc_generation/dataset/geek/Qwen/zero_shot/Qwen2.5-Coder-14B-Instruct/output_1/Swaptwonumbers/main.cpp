json
{
  "inputs": ["a", "b"],
  "outputs": ["out_a", "out_b"],
  "summary": {
    "line_1": "Include standard library headers.",
    "line_2": "Use the standard namespace.",
    "line_4": "Define the main function where the program execution begins.",
    "line_6": "Declare and initialize integer variables 'a' and 'b' with values 2 and 3 respectively.",
    "line_8": "Output the initial values of 'a' and 'b' before swapping.",
    "line_10": "Declare a temporary integer variable 'temp'.",
    "line_12": "Assign the value of 'a' to 'temp'.",
    "line_13": "Assign the value of 'b' to 'a'.",
    "line_14": "Assign the value stored in 'temp' to 'b'.",
    "line_16": "Output the values of 'a' and 'b' after swapping.",
    "line_18": "Return 0 to indicate successful execution of the program."
  },
  "systemc_code": {
    "code": "// SystemC program to swap two numbers using 3rd variable\n#include <systemc.h>\n\n
SC_MODULE(Swapper) {\n    sc_in<int> a;\n    sc_in<int> b;\n    sc_out<int> out_a;\n    sc_out<int> out_b;\n\n    SC_CTOR(Swapper) {\n        SC_METHOD(swap);\n        sensitive << a << b;\n    }\n\n    void swap() {\n        int temp;\n        temp = a.read();\n        out_a.write(b.read());\n        out_b.write(temp);\n    }\n};\n\n

SC_MODULE(Testbench) {
    sc_signal<int> a;      // Signal for the first number.
    sc_signal<int> b;      // Signal for the second number.
    sc_signal<int> out_a;  // Signal for the swapped output of a.
    sc_signal<int> out_b;  // Signal for the swapped output of b.

    Swapper swapper_inst;  // Instance of the Swapper module.

    SC_CTOR(Testbench) : swapper_inst("swapper_inst") {
        // Connect the signals to the Swapper module ports.
        swapper_inst.a(a);
        swapper_inst.b(b);
        swapper_inst.out_a(out_a);
        swapper_inst.out_b(out_b);

        // Create a thread to run test cases.
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Set initial values (equivalent to a = 2, b = 3 in the C++ code).
        a.write(2);
        b.write(3);
        wait(1, SC_NS); // Allow time for the Swapper module to process the inputs.

        // Assert that the swap operation has been performed correctly.
        assert(out_a.read() == 3); // After swapping, out_a should be 3.
        assert(out_b.read() == 2); // After swapping, out_b should be 2.

        // Print the values before and after the swap.
        cout << "Before swapping a = " << a.read() << " , b = " << b.read() << endl;
        cout << "After swapping a = " << out_a.read() << " , b = " << out_b.read() << endl;
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation.
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the Testbench.
    sc_start();         // Start the simulation.
    return 0;
}
