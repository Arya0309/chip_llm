
#include <systemc.h>

// Module to perform simple arithmetic operations
SC_MODULE(SimpleCalculator) {
    sc_in<char> op;          // Input port for the operator
    sc_in<double> a;         // Input port for the first number
    sc_in<double> b;         // Input port for the second number
    sc_out<double> result;   // Output port for the result

    // Constructor
    SC_CTOR(SimpleCalculator) {
        // Process to perform arithmetic operation
        SC_METHOD(calculate);
        sensitive << op << a << b;
    }

    // Method to perform the arithmetic operation
    void calculate() {
        double res;
        switch (op.read()) {
            case '+':
                res = a.read() + b.read();
                break;
            case '-':
                res = a.read() - b.read();
                break;
            case '*':
                res = a.read() * b.read();
                break;
            case '/':
                res = a.read() / b.read();
                break;
            default:
                cout << "Error! Operator is not correct" << endl;
                res = -DBL_MAX;
        }
        result.write(res);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<char> op_sig;       // Signal for the operator
    sc_signal<double> a_sig;      // Signal for the first number
    sc_signal<double> b_sig;      // Signal for the second number
    sc_signal<double> result_sig; // Signal for the result

    // Instance of the SimpleCalculator module
    SimpleCalculator calc_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : calc_inst("calc_inst") {
        // Connect signals to module ports
        calc_inst.op(op_sig);
        calc_inst.a(a_sig);
        calc_inst.b(b_sig);
        calc_inst.result(result_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Addition
        op_sig.write('+');
        a_sig.write(5.0);
        b_sig.write(3.0);
        wait(1, SC_NS); // Wait for the process to update
        double result = result_sig.read();
        cout << "5.0 + 3.0 = " << result << endl;
        assert(abs(result - 8.0) < 1e-9);

        // Test case 2: Subtraction
        op_sig.write('-');
        a_sig.write(10.0);
        b_sig.write(4.0);
        wait(1, SC_NS);
        result = result_sig.read();
        cout << "10.0 - 4.0 = " << result << endl;
        assert(abs(result - 6.0) < 1e-9);

        // Test case 3: Multiplication
        op_sig.write('*');
        a_sig.write(6.0);
        b_sig.write(7.0);
        wait(1, SC_NS);
        result = result_sig.read();
        cout << "6.0 * 7.0 = " << result << endl;
        assert(abs(result - 42.0) < 1e-9);

        // Test case 4: Division
        op_sig.write('/');
        a_sig.write(8.0);
        b_sig.write(2.0);
        wait(1, SC_NS);
        result = result_sig.read();
        cout << "8.0 / 2.0 = " << result << endl;
        assert(abs(result - 4.0) < 1e-9);

        // Test case 5: Invalid operator
        op_sig.write('%');
        a_sig.write(1.0);
        b_sig.write(1.0);
        wait(1, SC_NS);
        result = result_sig.read();
        cout << "Invalid operator '%' test passed." << endl;
        assert(result == -DBL_MAX);

        // End simulation after successful test
        cout << "All tests passed successfully." << endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
