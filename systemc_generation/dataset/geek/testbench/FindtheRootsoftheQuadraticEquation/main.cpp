#include <systemc.h>

// ─────────────── QuadraticRoots Module ───────────────
SC_MODULE(QuadraticRoots) {
    sc_in<int> a, b, c;           // Coefficients of the quadratic equation
    sc_out<double> root1, root2;  // Roots (real parts)
    sc_out<double> imag;          // Imaginary part (only used for complex roots)
    sc_out<bool> is_complex;      // Flag to indicate complex roots

    SC_CTOR(QuadraticRoots) {
        SC_METHOD(compute_roots);
        sensitive << a << b << c;
        dont_initialize();
    }

    void compute_roots() {
        int a_val = a.read();
        int b_val = b.read();
        int c_val = c.read();

        if (a_val == 0) {
            SC_REPORT_ERROR("QuadraticRoots", "Coefficient 'a' cannot be zero.");
            return;
        }

        int d = b_val * b_val - 4 * a_val * c_val;
        double sqrt_val = std::sqrt(std::abs(d));

        if (d > 0) {
            is_complex.write(false);
            root1.write((-b_val + sqrt_val) / (2.0 * a_val));
            root2.write((-b_val - sqrt_val) / (2.0 * a_val));
            imag.write(0);
        } else if (d == 0) {
            is_complex.write(false);
            double r = -b_val / (2.0 * a_val);
            root1.write(r);
            root2.write(r);
            imag.write(0);
        } else {
            is_complex.write(true);
            double real_part = -b_val / (2.0 * a_val);
            double imag_part = sqrt_val / (2.0 * a_val);
            root1.write(real_part);
            root2.write(real_part);
            imag.write(imag_part);
        }
    }
};

// ─────────────── Testbench ───────────────
SC_MODULE(Testbench) {
    sc_signal<int> a_sig, b_sig, c_sig;
    sc_signal<double> root1_sig, root2_sig, imag_sig;
    sc_signal<bool> is_complex_sig;

    QuadraticRoots dut;

    SC_CTOR(Testbench) : dut("QuadraticRoots") {
        dut.a(a_sig);
        dut.b(b_sig);
        dut.c(c_sig);
        dut.root1(root1_sig);
        dut.root2(root2_sig);
        dut.imag(imag_sig);
        dut.is_complex(is_complex_sig);

        SC_THREAD(run_test);
    }

    void run_test() {
        a_sig = 1;
        b_sig = -7;
        c_sig = 12; // Expected roots: 4 and 3 (real and distinct)
        wait(1, SC_NS);  // 等一個模擬時間單位讓 SC_METHOD 完成運算
;

        assert(is_complex_sig.read() == false);
        double r1 = root1_sig.read();
        double r2 = root2_sig.read();

        // Order of roots can vary
        bool case1 = std::abs(r1 - 4.0) < 1e-6 && std::abs(r2 - 3.0) < 1e-6;
        bool case2 = std::abs(r1 - 3.0) < 1e-6 && std::abs(r2 - 4.0) < 1e-6;

        std::cout << "Real roots: " << r1 << " and " << r2 << "\n";

        assert(is_complex_sig.read() == false);  // 確認為實根
        assert(std::abs(r1 - 4.0) < 1e-6 || std::abs(r1 - 3.0) < 1e-6); // r1 正確
        assert(std::abs(r2 - 4.0) < 1e-6 || std::abs(r2 - 3.0) < 1e-6); // r2 正確
        assert(case1 || case2); // 根組合正確

        std::cout << "Test passed successfully." << std::endl;

        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
