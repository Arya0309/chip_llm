#include <systemc.h>

// ─────────────── Calculator Module ───────────────
SC_MODULE(Calculator) {
    sc_in<char>  op;     // Operator: +, -, *, /
    sc_in<double> a, b;  // Operands
    sc_out<double> res;  // Result
    sc_out<bool> valid;  // Valid result indicator

    SC_CTOR(Calculator) {
        SC_METHOD(compute);
        sensitive << op << a << b;
        dont_initialize();
    }

    void compute() {
        double result = 0.0;
        bool ok = true;

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
                if (b.read() != 0)
                    result = a.read() / b.read();
                else {
                    ok = false;
                    result = 0.0;
                }
                break;
            default:
                ok = false;
                result = 0.0;
        }

        res.write(result);
        valid.write(ok);
    }
};

// ─────────────── Testbench ───────────────
SC_MODULE(Testbench) {
    sc_signal<char>  op_sig;
    sc_signal<double> a_sig, b_sig, res_sig;
    sc_signal<bool> valid_sig;

    Calculator calc_inst;

    SC_CTOR(Testbench) : calc_inst("Calculator") {
        calc_inst.op(op_sig);
        calc_inst.a(a_sig);
        calc_inst.b(b_sig);
        calc_inst.res(res_sig);
        calc_inst.valid(valid_sig);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        struct TestCase {
            char op;
            double a, b;
            double expected;
            bool valid;
        };

        TestCase cases[] = {
            {'+', 3.0, 4.0, 7.0, true},
            {'-', 10.0, 2.5, 7.5, true},
            {'*', 5.0, 6.0, 30.0, true},
            {'/', 9.0, 3.0, 3.0, true},
            {'/', 5.0, 0.0, 0.0, false},
            {'%', 8.0, 3.0, 0.0, false} // Invalid operator
        };

        for (const auto& test : cases) {
            op_sig.write(test.op);
            a_sig.write(test.a);
            b_sig.write(test.b);
            wait(1, SC_NS);

            std::cout << "Test: " << test.a << " " << test.op << " " << test.b << "\n";
            std::cout << "→ Result = " << res_sig.read()
                      << ", Valid = " << valid_sig.read() << "\n";

            assert(valid_sig.read() == test.valid);
            if (test.valid) {
                assert(fabs(res_sig.read() - test.expected) < 1e-6);
            }
        }

        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

// ─────────────── sc_main ───────────────
int sc_main(int, char*[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
