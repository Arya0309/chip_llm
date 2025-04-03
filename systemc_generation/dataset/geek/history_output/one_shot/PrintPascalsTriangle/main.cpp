
#include <systemc.h>

// Module to generate Pascal's Triangle
SC_MODULE(PascalsTriangleGenerator) {
    sc_in<int> n;          // Input: Number of rows
    sc_out<sc_bigint<100>> triangle; // Output: Pascal's Triangle as a concatenated string

    // Constructor
    SC_CTOR(PascalsTriangleGenerator) {
        // Process to generate Pascal's Triangle
        SC_METHOD(generatePascalTriangle);
        sensitive << n;
    }

    // Method to calculate binomial coefficient
    int binomialCoeff(int n, int k) {
        int res = 1;
        if (k > n - k)
            k = n - k;
        for (int i = 0; i < k; ++i) {
            res *= (n - i);
            res /= (i + 1);
        }
        return res;
    }

    // Method to generate Pascal's Triangle
    void generatePascalTriangle() {
        sc_bigint<100> result = 0; // Using sc_bigint to handle large numbers
        int shift = 0;
        for (int line = 0; line < n.read(); line++) {
            for (int i = 0; i <= line; i++) {
                int coeff = binomialCoeff(line, i);
                result.range(shift + 9, shift) = coeff;
                shift += 10; // Assuming each coefficient is represented in 10 bits
            }
            shift += 10; // New line separator
        }
        triangle.write(result);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n; // Signal for the number of rows
    sc_signal<sc_bigint<100>> triangle; // Signal for the generated Pascal's Triangle

    PascalsTriangleGenerator generator_inst; // Instance of the generator module

    // Constructor
    SC_CTOR(Testbench) : generator_inst("generator") {
        // Connect signals to ports
        generator_inst.n(n);
        generator_inst.triangle(triangle);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        n = 7; // Set the number of rows
        wait(1, SC_NS); // Wait for the generator to process

        // Read and print the result
        sc_bigint<100> result = triangle.read();
        int shift = 0;
        for (int line = 0; line < n.read(); line++) {
            for (int i = 0; i <= line; i++) {
                int coeff = result.range(shift + 9, shift).to_int();
                cout << coeff << " ";
                shift += 10;
            }
            cout << "\n";
            shift += 10; // Skip the new line separator
        }

        // Print success message
        cout << "Pascal's Triangle generated successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
