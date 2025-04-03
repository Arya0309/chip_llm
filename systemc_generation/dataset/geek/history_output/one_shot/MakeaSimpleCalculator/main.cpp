
#include <systemc.h>

// Module that performs simple arithmetic operations
SC_MODULE(SimpleCalculator) {
    sc_in<char> op;          // Input port for the operator
    sc_in<double> a;         // Input port for the first number
    sc_in<double> b;         // Input port for the second number
    sc_out<double> result;   // Output port for the result
    sc_out<bool> valid_op;    // Output port indicating if the operation was valid

    // Constructor
    SC_CTOR(SimpleCalculator) {
        // Process to perform the calculation
        SC_METHOD(calculate);
        sensitive << op << a << b;
    }

    // Method to perform the calculation
    void calculate() {
        double res;
        bool valid = true;

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
                if (b.read() != 0.0) {
                    res = a.read() / b.read();
                } else {
                    res = -DBL_MAX;
                    valid = false;
                }
                break;
            default:
                res = -DBL_MAX;
                valid = false;
        }

        result.write(res);
        valid_op.write(valid);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<char> op;       // Signal for the operator
    sc_signal<double> a;      // Signal for the first number
    sc_signal<double> b;      // Signal for the second number
    sc_signal<double> result; // Signal for the result
    sc_signal<bool> valid_op;  // Signal indicating if the operation was valid

    SimpleCalculator calc_inst; // Instance of the SimpleCalculator module

    // Constructor
    SC_CTOR(Testbench) : calc_inst("calc") {
        // Connect signals to ports
        calc_inst.op(op);
        calc_inst.a(a);
        calc_inst.b(b);
        calc_inst.result(result);
        calc_inst.valid_op(valid_op);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Addition
        op = '+'; a = 5.0; b = 3.0;
        wait(1, SC_NS);
        assert(valid_op.read() && result.read() == 8.0);
        cout << "Test 1: Result = " << result.read() << endl;

        // Test case 2: Subtraction
        op = '-'; a = 10.0; b = 4.0;
        wait(1, SC_NS);
        assert(valid_op.read() && result.read() == 6.0);
        cout << "Test 2: Result = " << result.read() << endl;

        // Test case 3: Multiplication
        op = '*'; a = 2.5; b = 4.0;
        wait(1, SC_NS);
        assert(valid_op.read() && result.read() == 10.0);
        cout << "Test 3: Result = " << result.read() << endl;

        // Test case 4: Division
        op = '/'; a = 8.0; b = 2.0;
        wait(1, SC_NS);
        assert(valid_op.read() && result.read() == 4.0);
        cout << "Test 4: Result = " << result.read() << endl;

        // Test case 5: Division by zero
        op = '/'; a = 8.0; b = 0.0;
        wait(1, SC_NS);
        assert(!valid_op.read() && result.read() == -DBL_MAX);
        cout << "Test 5: Division by zero detected." << endl;

        // Test case 6: Invalid operator
        op = '%'; a = 8.0; b = 2.0;
        wait(1, SC_NS);
        assert(!valid_op.read() && result.read() == -DBL_MAX);
        cout << "Test 6: Invalid operator detected." << endl;

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
