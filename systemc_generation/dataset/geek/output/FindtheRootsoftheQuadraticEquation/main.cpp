
#include <systemc.h>

// Module to find roots of a quadratic equation
SC_MODULE(QuadraticSolver) {
    sc_in<int> a;          // Input coefficient a
    sc_in<int> b;          // Input coefficient b
    sc_in<int> c;          // Input coefficient c
    sc_out<sc_bv<64>> roots; // Output roots (encoded as bit vector)

    // Constructor
    SC_CTOR(QuadraticSolver) {
        // Process to compute roots
        SC_METHOD(compute_roots);
        sensitive << a << b << c;
    }

    // Method to compute the roots
    void compute_roots() {
        int a_val = a.read();
        int b_val = b.read();
        int c_val = c.read();

        // Check if the equation is quadratic
        if (a_val == 0) {
            roots.write(sc_bv<64>("Invalid"));
            return;
        }

        // Calculate discriminant
        int d = b_val * b_val - 4 * a_val * c_val;
        double sqrt_val = sqrt(abs(d));

        sc_bv<64> root_info;
        if (d > 0) {
            double root1 = (-b_val + sqrt_val) / (2 * a_val);
            double root2 = (-b_val - sqrt_val) / (2 * a_val);
            root_info = sc_bv<64>(std::string("Real and different ") + std::to_string(root1) + " " + std::to_string(root2));
        } else if (d == 0) {
            double root = -(double)b_val / (2 * a_val);
            root_info = sc_bv<64>(std::string("Real and same ") + std::to_string(root));
        } else {
            double real_part = -(double)b_val / (2 * a_val);
            double imag_part = sqrt_val / (2 * a_val);
            root_info = sc_bv<64>(std::string("Complex ") + std::to_string(real_part) + " + i" + std::to_string(imag_part) + " " + std::to_string(real_part) + " - i" + std::to_string(imag_part));
        }

        roots.write(root_info);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a_sig; // Signal for coefficient a
    sc_signal<int> b_sig; // Signal for coefficient b
    sc_signal<int> c_sig; // Signal for coefficient c
    sc_signal<sc_bv<64>> roots_sig; // Signal for roots

    // Instance of the QuadraticSolver module
    QuadraticSolver solver_inst;

    // Constructor
    SC_CTOR(Testbench)
    : solver_inst("solver") {
        // Connect signals to module ports
        solver_inst.a(a_sig);
        solver_inst.b(b_sig);
        solver_inst.c(c_sig);
        solver_inst.roots(roots_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: a = 1, b = -7, c = 12
        a_sig.write(1);
        b_sig.write(-7);
        c_sig.write(12);
        wait(1, SC_NS); // Wait for the process to update
        sc_bv<64> result = roots_sig.read();
        std::cout << "Roots: " << result.to_string() << std::endl;

        // Additional test cases can be added here
        // For example:
        // a_sig.write(1);
        // b_sig.write(2);
        // c_sig.write(1);
        // wait(1, SC_NS);
        // result = roots_sig.read();
        // std::cout << "Roots: " << result.to_string() << std::endl;

        // End simulation after successful test
        std::cout << "Test completed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
