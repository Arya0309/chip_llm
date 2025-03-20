
#include <systemc.h>

// User Defined Calculator module
SC_MODULE(SimpleCalculator) {
    // Input ports for operator and operands
    sc_in<char> op;
    sc_in<double> operand1, operand2;
    
    // Output port for result
    sc_out<double> result;

    // Internal variable to store the result
    double res;

    // Process to perform arithmetic operations
    void calculate() {
        // Reading inputs
        char operation = op.read();
        double a = operand1.read();
        double b = operand2.read();

        // Define all four operations in the corresponding switch-case
        switch (operation) {
        case '+':
            res = a + b;
            break;
        case '-':
            res = a - b;
            break;
        case '*':
            res = a * b;
            break;
        case '/':
            if (b != 0.0)
                res = a / b;
            else {
                cout << "Error! Division by zero.";
                res = -DBL_MAX;
            }
            break;
        default:
            cout << "Error! Operator is not correct";
            res = -DBL_MAX;
        }

        // Writing output
        result.write(res);
    }

    // Constructor to register the process
    SC_CTOR(SimpleCalculator) {
        SC_METHOD(calculate);
        sensitive << op << operand1 << operand2;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with SimpleCalculator
    sc_signal<char> op;
    sc_signal<double> operand1, operand2;
    sc_signal<double> result;

    // Instance of SimpleCalculator
    SimpleCalculator calculator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing values
        op = '+'; operand1 = 5.0; operand2 = 3.0;
        wait(1, SC_NS); // Wait for one delta cycle to let the calculator process

        // Print the results
        cout << "Operation: " << op.read() << endl;
        cout << "Operands: " << operand1.read() << ", " << operand2.read() << endl;
        cout << "Result: " << result.read() << endl;

        // Change operation and operands
        op = '*'; operand1 = 4.0; operand2 = 2.0;
        wait(1, SC_NS); // Wait for one delta cycle to let the calculator process

        // Print the results
        cout << "Operation: " << op.read() << endl;
        cout << "Operands: " << operand1.read() << ", " << operand2.read() << endl;
        cout << "Result: " << result.read() << endl;

        // Change operation and operands
        op = '/'; operand1 = 10.0; operand2 = 2.0;
        wait(1, SC_NS); // Wait for one delta cycle to let the calculator process

        // Print the results
        cout << "Operation: " << op.read() << endl;
        cout << "Operands: " << operand1.read() << ", " << operand2.read() << endl;
        cout << "Result: " << result.read() << endl;

        // Change operation and operands to test error handling
        op = '/'; operand1 = 10.0; operand2 = 0.0;
        wait(1, SC_NS); // Wait for one delta cycle to let the calculator process

        // Print the results
        cout << "Operation: " << op.read() << endl;
        cout << "Operands: " << operand1.read() << ", " << operand2.read() << endl;
        cout << "Result: " << result.read() << endl;

        // Change operation and operands to test error handling
        op = '%'; operand1 = 10.0; operand2 = 2.0;
        wait(1, SC_NS); // Wait for one delta cycle to let the calculator process

        // Print the results
        cout << "Operation: " << op.read() << endl;
        cout << "Operands: " << operand1.read() << ", " << operand2.read() << endl;
        cout << "Result: " << result.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : calculator("calculator") {
        // Connecting signals to the calculator ports
        calculator.op(op); calculator.operand1(operand1); calculator.operand2(operand2);
        calculator.result(result);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
