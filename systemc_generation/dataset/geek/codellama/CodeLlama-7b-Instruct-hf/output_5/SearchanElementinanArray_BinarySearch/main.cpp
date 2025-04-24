sc
#include <systemc.h>

// Binary search module

SC_MODULE(BinarySearchModule) {
  // Target value input
  sc_in<int> target;
  // Result output
  sc_out<bool> found;
  // Input vector
  sc_vector<sc_in<int>> v;
  // Current search range low and high values
  sc_in<int> low, high;

  // Reset low and high to 0 and vector size - 1 respectively
  SC_CTOR(BinarySearchModule) : 
    target("target"), found("found"), v("v"), low("low"), high("high") {
    // Initialize low and high to 0 and vector size - 1 respectively
    low.write(0); high.write(v.size() - 1);
  }

  // Search function
  void search() {
    // Keep searching till the range is not 0
    while (low.read() <= high.read()) {
      // Calculate mid point
      int mid = (low.read() + high.read()) / 2;
      // If the middle element is equal to target
      if (v[mid].read() == target.read()) {
        // Set found flag to true
        found.write(true);
        // Exit the search
        return;
      }
      // If the middle element is greater than target
      if (v[mid].read() > target.read()) {
        // Update high to mid - 1
        high.write(mid - 1);
      }
      // If the middle element is smaller than target
      else {
        // Update low to mid + 1
        low.write(mid + 1);
      }
    }
    // If we reach here, the target is not present in the vector
    found.write(false);
  }

  // Entry point
  SC_CTOR(main) : target("target"), found("found"), v("v") {
    search();
  }
};


SC_MODULE(Testbench) {
    sc_vector<sc_signal<int>> v_sig; // Signals for the vector elements
    sc_signal<int> target_sig;       // Signal for the target value
    sc_signal<bool> found_sig;       // Signal for the result

    BinarySearchModule bs_inst; // Instance of the BinarySearchModule

    // Constructor
    SC_CTOR(Testbench)
    : bs_inst("bs_inst"), v_sig("v_sig", 8) { // Initialize vector size to 8
        // Connect signals to module ports
        for (size_t i = 0; i < v_sig.size(); ++i) {
            bs_inst.v[i](v_sig[i]);
        }
        bs_inst.target(target_sig);
        bs_inst.found(found_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize the vector with sorted values
        std::vector<int> v_values = {1, 2, 3, 4, 5, 8, 9, 11};
        for (size_t i = 0; i < v_values.size(); ++i) {
            v_sig[i].write(v_values[i]);
        }

        // Test case 1: Target found
        target_sig.write(8);
        wait(1, SC_NS); // Wait for the module to process
        assert(found_sig.read() == true);
        cout << "Target " << target_sig.read() << " found: " << found_sig.read() << endl;

        // Test case 2: Target not found
        target_sig.write(7);
        wait(1, SC_NS); // Wait for the module to process
        assert(found_sig.read() == false);
        cout << "Target " << target_sig.read() << " found: " << found_sig.read() << endl;

        // End simulation after successful test
        cout << "All tests passed successfully." << endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
