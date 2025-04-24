c
#include <systemc.h>

// User-defined function to multiply two floats
float mul_floatnumbers(float a, float b) {
  return a * b;
}

// SystemC module that connects the input/output ports

SC_MODULE(Multiplier) {
  sc_in<float> a;  // Input port for the first number
  sc_in<float> b;  // Input port for the second number
  sc_out<float> product; // Output port for the product

  // Constructor
  SC_CTOR(Multiplier) {
    // Process to perform multiplication
    SC_METHOD(mul_floatnumbers);
    sensitive << a << b;
  }

  // Method to multiply the two numbers
  void mul_floatnumbers() {
    product.write(a.read() * b.read());
  }
};

int main() {
  // Declare and initialize floats A and B
  float A = 1.2, B = 3.0;

  // Create an instance of the SystemC module
  Multiplier my_module;

  // Connect the input ports
  my_module.a = A;
  my_module.b = B;

  // Call the multiplication function in the SystemC module
  my_module.mul_floatnumbers();

  // Get the output from the SystemC module
  float product = my_module.product;

  // Print the output
  cout << product << endl;

  return 0;
}


SC_MODULE(Testbench) {
    // Signals to connect to the Multiplier module ports
    sc_signal<float> a;
    sc_signal<float> b;
    sc_signal<float> product;

    // Instance of the Multiplier module
    Multiplier multiplier_inst;

    // Constructor that sets up the testbench
    SC_CTOR(Testbench)
    : multiplier_inst("multiplier_inst") {
        // Connect signals to the multiplier's ports
        multiplier_inst.a(a);
        multiplier_inst.b(b);
        multiplier_inst.product(product);

        // Create a thread to run the test cases
        SC_THREAD(run_tests);
    }

    // Thread that runs test cases for the Multiplier module
    void run_tests() {
        const float epsilon = 1e-6;

        // Test case 1: Multiply 1.2 and 3.0 (Expected product: 3.6)
        a.write(1.2);
        b.write(3.0);
        wait(1, SC_NS);
        float result = product.read();
        std::cout << "1.2 * 3.0 = " << result << std::endl;
        assert(std::fabs(result - 3.6) < epsilon);

        // Test case 2: Multiply 2.5 and 4.0 (Expected product: 10.0)
        a.write(2.5);
        b.write(4.0);
        wait(1, SC_NS);
        result = product.read();
        std::cout << "2.5 * 4.0 = " << result << std::endl;
        assert(std::fabs(result - 10.0) < epsilon);

        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    // Instantiate the testbench module
    Testbench tb("tb");

    // Start the simulation
    sc_start();

    return 0;
}
