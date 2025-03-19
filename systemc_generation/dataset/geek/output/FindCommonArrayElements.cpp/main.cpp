
#include <systemc.h>
#include <algorithm>
#include <vector>

// SystemC module to find common elements between two arrays
SC_MODULE(CommonElementsFinder) {
    // Input ports for the arrays and their sizes
    sc_in<sc_uint<32>> size1, size2;
    sc_in<sc_int<32>> arr1[10], arr2[10]; // Assuming max size of 10 for simplicity

    // Output port for the number of common elements found
    sc_out<sc_uint<32>> num_common_elements;
    // Output port for the common elements
    sc_out<sc_int<32>> common_elements[10];

    // Internal variables
    sc_uint<32> n1, n2;
    sc_vector<sc_int<32>> sorted_arr1, sorted_arr2;
    std::vector<int> result;

    // Process to find common elements
    void findCommonElements() {
        // Read sizes
        n1 = size1.read();
        n2 = size2.read();

        // Read arrays into vectors
        sorted_arr1.init(n1);
        sorted_arr2.init(n2);
        for (int i = 0; i < n1; ++i) {
            sorted_arr1[i] = arr1[i].read();
        }
        for (int i = 0; i < n2; ++i) {
            sorted_arr2[i] = arr2[i].read();
        }

        // Sort arrays
        std::sort(sorted_arr1.begin(), sorted_arr1.end());
        std::sort(sorted_arr2.begin(), sorted_arr2.end());

        // Find intersection
        std::set_intersection(sorted_arr1.begin(), sorted_arr1.end(),
                              sorted_arr2.begin(), sorted_arr2.end(),
                              std::back_inserter(result));

        // Write number of common elements
        num_common_elements.write(result.size());

        // Write common elements
        for (int i = 0; i < result.size(); ++i) {
            common_elements[i].write(result[i]);
        }
    }

    // Constructor to register the process
    SC_CTOR(CommonElementsFinder) {
        SC_METHOD(findCommonElements);
        sensitive << size1 << size2;
        for (int i = 0; i < 10; ++i) {
            sensitive << arr1[i];
            sensitive << arr2[i];
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with CommonElementsFinder
    sc_signal<sc_uint<32>> size1, size2;
    sc_signal<sc_int<32>> arr1[10], arr2[10];
    sc_signal<sc_uint<32>> num_common_elements;
    sc_signal<sc_int<32>> common_elements[10];

    // Instance of CommonElementsFinder
    CommonElementsFinder finder;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize arrays and sizes
        size1 = 6;
        size2 = 6;
        arr1[0] = 1; arr1[1] = 45; arr1[2] = 54; arr1[3] = 71; arr1[4] = 76; arr1[5] = 12;
        arr2[0] = 1; arr2[1] = 7; arr2[2] = 5; arr2[3] = 4; arr2[4] = 6; arr2[5] = 12;

        // Wait for one delta cycle to let the finder process
        wait(1, SC_NS);

        // Print the number of common elements
        cout << "Number of common elements: " << num_common_elements.read() << endl;

        // Print the common elements
        cout << "Common elements: ";
        for (int i = 0; i < num_common_elements.read(); ++i) {
            cout << common_elements[i].read() << " ";
        }
        cout << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : finder("finder") {
        // Connecting signals to the finder ports
        finder.size1(size1); finder.size2(size2);
        for (int i = 0; i < 10; ++i) {
            finder.arr1[i](arr1[i]); finder.arr2[i](arr2[i]);
            finder.common_elements[i](common_elements[i]);
        }
        finder.num_common_elements(num_common_elements);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
