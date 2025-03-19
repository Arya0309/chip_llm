
#include <systemc.h>

SC_MODULE(SimpleCalculator) {
    sc_in<char> op;            // Input operator
    sc_in<double> a, b;        // Input operands
    sc_out<double> res;         // Output result
    sc_out<bool> valid;        // Validity flag for result

    SC_CTOR(SimpleCalculator) {
        SC_METHOD(calculate);
        sensitive << op << a << b;
    }

    void calculate() {
        double result;
        bool isValid = true;

        switch (op.read()) {
            case '+':
                result = a.read() + b.read();
                break;
            case '-':
                result = a.read() - b.read();
                break;
            case '*':
                result = a.read() * b.read();
                break;
            case '/':
                if (b.read() != 0.0) {
                    result = a.read() / b.read();
                } else {
                    result = -DBL_MAX;
                    isValid = false;
                }
                break;
            default:
                result = -DBL_MAX;
                isValid = false;
        }

        res.write(result);
        valid.write(isValid);
    }
};

int sc_main(int argc, char* argv[]) {
    SimpleCalculator calc("calculator");

    sc_signal<char> op;
    sc_signal<double> a, b, res;
    sc_signal<bool> valid;

    calc.op(op);
    calc.a(a);
    calc.b(b);
    calc.res(res);
    calc.valid(valid);

    // Simulate some test cases
    sc_start(0, SC_NS); // Start simulation at time 0

    op.write('+'); a.write(5.0); b.write(3.0); sc_start(1, SC_NS);
    cout << "Op: +, A: 5.0, B: 3.0 => Result: " << res.read() << ", Valid: " << valid.read() << endl;

    op.write('-'); a.write(5.0); b.write(3.0); sc_start(1, SC_NS);
    cout << "Op: -, A: 5.0, B: 3.0 => Result: " << res.read() << ", Valid: " << valid.read() << endl;

    op.write('*'); a.write(5.0); b.write(3.0); sc_start(1, SC_NS);
    cout << "Op: *, A: 5.0, B: 3.0 => Result: " << res.read() << ", Valid: " << valid.read() << endl;

    op.write('/'); a.write(5.0); b.write(3.0); sc_start(1, SC_NS);
    cout << "Op: /, A: 5.0, B: 3.0 => Result: " << res.read() << ", Valid: " << valid.read() << endl;

    op.write('/'); a.write(5.0); b.write(0.0); sc_start(1, SC_NS);
    cout << "Op: /, A: 5.0, B: 0.0 => Result: " << res.read() << ", Valid: " << valid.read() << endl;

    op.write('%'); a.write(5.0); b.write(3.0); sc_start(1, SC_NS);
    cout << "Op: %, A: 5.0, B: 3.0 => Result: " << res.read() << ", Valid: " << valid.read() << endl;

    return 0;
}
