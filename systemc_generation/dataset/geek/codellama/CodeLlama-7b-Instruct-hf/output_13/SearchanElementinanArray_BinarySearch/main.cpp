scss
#include <systemc.h>

// Module that implements an iterative binary search

SC_MODULE(BinarySearchModule) {
    sc_in<int> target;  // Vector element to search for
    sc_out<bool> found;  // Output indicating whether the element was found
    sc_vector<sc_in<int>> v;  // Vector to search in
  
    // Searches for the target element in the vector
    void search() {
        // Defining the part of the vector to be searched
        int low = 0, high = v.size() - 1;
      
        // Till the element is found or vector cannot be divided into more parts
        while (low <= high) {
          
            // Finding the middle element of the current vector segment
            int mid = (int)((high + low) / 2);

            // If the middle element is equal to the target, set found to 1 and exit the loop
            if (mid == target) {
                found.write(1);
                loop_done = true;
                break;
            }

            // If the middle element is greater than the target, set the upper bound to the middle element minus one
            if (mid > target) {
                high = mid - 1;
            }
            
            // If the middle element is smaller than the target, set the lower bound to the middle element plus one
            else {
                low = mid + 1;
            }
        }
      
      // If we don't find the target, set found to 0
      found.write(0);
    }
};

// Module that performs a binary search and displays a message indicating whether the target was found

SC_MODULE(main) {
    sc_in<int> target;  // Target element to search for
    sc_out<bool> found;  // Output indicating whether the element was found
    sc_vector<sc_in<int>> v;  // Vector to search in
  
    // Initiates the search
    void search() {
        BinarySearchModule searcher(v, target);
        found.write(searcher.found());
        std::cout << (found.read() ? "Target Found" : "Target Not Found");
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
