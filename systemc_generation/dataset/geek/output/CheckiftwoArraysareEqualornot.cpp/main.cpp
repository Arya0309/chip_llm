
#include <systemc.h>

// Module to check if two arrays are equal
SC_MODULE(ArrayChecker) {
    // Input ports for the arrays and their sizes
    sc_in<sc_int<32>> arr1[5];
    sc_in<sc_int<32>> arr2[5];
    sc_in<int> size1;
    sc_in<int> size2;
    
    // Output port for the result
    sc_out<bool> areEqual;

    // Process to check if arrays are equal
    void checkEquality() {
        // Read the sizes
        int n = size1.read();
        int m = size2.read();

        // If lengths of array are not equal, arrays are not equal
        if (n != m) {
            areEqual.write(false);
            return;
        }

        // Create local copies of arrays to sort
        sc_int<32> sortedArr1[5];
        sc_int<32> sortedArr2[5];
        for (int i = 0; i < n; ++i) {
            sortedArr1[i] = arr1[i].read();
            sortedArr2[i] = arr2[i].read();
        }

        // Sort both arrays
        std::sort(sortedArr1, sortedArr1 + n);
        std::sort(sortedArr2, sortedArr2 + m);

        // Linearly compare elements
        for (int i = 0; i < n; i++) {
            if (sortedArr1[i] != sortedArr2[i]) {
                areEqual.write(false);
                return;
            }
        }

        // If elements are same
        areEqual.write(true);
    }

    // Constructor to register the process
    SC_CTOR(ArrayChecker) {
        SC_METHOD(checkEquality);
        sensitive << size1 << size2;
        for (int i = 0; i < 5; ++i) {
            sensitive << arr1[i] << arr2[i];
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with ArrayChecker
    sc_signal<sc_int<32>> arr1[5];
    sc_signal<sc_int<32>> arr2[5];
    sc_signal<int> size1, size2;
    sc_signal<bool> areEqual;

    // Instance of ArrayChecker
    ArrayChecker checker;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize arrays
        arr1[0] = 1; arr1[1] = 2; arr1[2] = 3; arr1[3] = 4; arr1[4] = 5;
        arr2[0] = 5; arr2[1] = 4; arr2[2] = 3; arr2[3] = 2; arr2[4] = 1;
        size1 = 5;
        size2 = 5;

        // Wait for one delta cycle to let the checker process
        wait(1, SC_NS);

        // Print the result
        if (areEqual.read()) {
            cout << "Equal" << endl;
        } else {
            cout << "Not Equal" << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : checker("checker") {
        // Connecting signals to the checker ports
        for (int i = 0; i < 5; ++i) {
            checker.arr1[i](arr1[i]);
            checker.arr2[i](arr2[i]);
        }
        checker.size1(size1);
        checker.size2(size2);
        checker.areEqual(areEqual);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
