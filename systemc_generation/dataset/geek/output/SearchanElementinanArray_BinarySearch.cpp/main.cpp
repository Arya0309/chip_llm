
#include <systemc.h>

// BinarySearch module
SC_MODULE(BinarySearch) {
    // Input port for the target value
    sc_in<int> target;
    // Output port indicating whether the target is found
    sc_out<bool> found;
    // Input vector (as an array for simplicity)
    int v[8];
    int size;

    // Process to perform binary search
    void doBinarySearch() {
        int low = 0, high = size - 1;
        bool result = false;

        while (low <= high && !result) {
            int mid = ((high - low) / 2) + low;
            if (v[mid] == target.read()) {
                result = true;
            } else if (v[mid] > target.read()) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        found.write(result);
    }

    // Constructor to register the process
    SC_CTOR(BinarySearch) : size(8) {
        SC_METHOD(doBinarySearch);
        sensitive << target;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with BinarySearch
    sc_signal<int> target;
    sc_signal<bool> found;

    // Instance of BinarySearch
    BinarySearch searcher;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the vector in the searcher
        int values[] = {1, 2, 3, 4, 5, 8, 9, 11};
        for (int i = 0; i < 8; ++i) {
            searcher.v[i] = values[i];
        }

        // Set the target to search
        target = 8;

        // Wait for one delta cycle to let the searcher process
        wait(1, SC_NS);

        // Print the result
        if (found.read()) {
            cout << target.read() << " found." << endl;
        } else {
            cout << target.read() << " NOT found." << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : searcher("searcher") {
        // Connecting signals to the searcher ports
        searcher.target(target);
        searcher.found(found);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
