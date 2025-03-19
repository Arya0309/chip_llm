
#include <systemc.h>

SC_MODULE(QuadraticSolver) {
    sc_in<int> a;
    sc_in<int> b;
    sc_in<int> c;
    sc_out<sc_bv<64>> root1_real; // Using 64-bit binary vector for double precision
    sc_out<sc_bv<64>> root1_imag;
    sc_out<sc_bv<64>> root2_real;
    sc_out<sc_bv<64>> root2_imag;
    sc_out<bool> valid;

    SC_CTOR(QuadraticSolver) {
        SC_METHOD(solve);
        sensitive << a << b << c;
    }

    void solve() {
        int a_val = a.read();
        int b_val = b.read();
        int c_val = c.read();

        if (a_val == 0) {
            valid.write(false);
            return;
        }

        int d = b_val * b_val - 4 * a_val * c_val;
        double sqrt_val = sqrt(abs(d));

        if (d > 0) {
            double r1_real = (-b_val + sqrt_val) / (2 * a_val);
            double r2_real = (-b_val - sqrt_val) / (2 * a_val);
            root1_real.write(sc_bv<64>(*(reinterpret_cast<uint64_t*>(&r1_real))));
            root1_imag.write(sc_bv<64>(0));
            root2_real.write(sc_bv<64>(*(reinterpret_cast<uint64_t*>(&r2_real))));
            root2_imag.write(sc_bv<64>(0));
        } else if (d == 0) {
            double r_real = -(double)b_val / (2 * a_val);
            root1_real.write(sc_bv<64>(*(reinterpret_cast<uint64_t*>(&r_real))));
            root1_imag.write(sc_bv<64>(0));
            root2_real.write(sc_bv<64>(*(reinterpret_cast<uint64_t*>(&r_real))));
            root2_imag.write(sc_bv<64>(0));
        } else {
            double r_real = -(double)b_val / (2 * a_val);
            double r_imag = sqrt_val / (2 * a_val);
            root1_real.write(sc_bv<64>(*(reinterpret_cast<uint64_t*>(&r_real))));
            root1_imag.write(sc_bv<64>(*(reinterpret_cast<uint64_t*>(&r_imag))));
            root2_real.write(sc_bv<64>(*(reinterpret_cast<uint64_t*>(&r_real))));
            root2_imag.write(sc_bv<64>(*(reinterpret_cast<uint64_t*>(&(0 - r_imag))));
        }

        valid.write(true);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<int> a, b, c;
    sc_signal<sc_bv<64>> root1_real, root1_imag, root2_real, root2_imag;
    sc_signal<bool> valid;

    QuadraticSolver solver("solver");
    solver.a(a);
    solver.b(b);
    solver.c(c);
    solver.root1_real(root1_real);
    solver.root1_imag(root1_imag);
    solver.root2_real(root2_real);
    solver.root2_imag(root2_imag);
    solver.valid(valid);

    a.write(1);
    b.write(-7);
    c.write(12);

    sc_start();

    if (valid.read()) {
        double r1_real = *(reinterpret_cast<double*>(&root1_real.read().to_uint64()));
        double r1_imag = *(reinterpret_cast<double*>(&root1_imag.read().to_uint64()));
        double r2_real = *(reinterpret_cast<double*>(&root2_real.read().to_uint64()));
        double r2_imag = *(reinterpret_cast<double*>(&root2_imag.read().to_uint64()));

        if (r1_imag == 0 && r2_imag == 0) {
            if (r1_real == r2_real) {
                cout << "Roots are real and same " << r1_real << endl;
            } else {
                cout << "Roots are real and different " << r1_real << " " << r2_real << endl;
            }
        } else {
            cout << "Roots are complex " << r1_real << " + i" << r1_imag << " " << r2_real << " - i" << r2_imag << endl;
        }
    } else {
        cout << "Invalid" << endl;
    }

    return 0;
}
