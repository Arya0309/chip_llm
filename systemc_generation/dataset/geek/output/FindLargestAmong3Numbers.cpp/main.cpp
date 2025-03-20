
#include <systemc.h>

// Module to find the largest among three numbers
SC_MODULE(FindLargest) {
    // Input ports for three integers
    sc_in<int> a, b, c;
    
    // Output port for the largest number
    sc_out<int> largest;

    // Process to determine the largest number
    void findLargestNumber() {
        // Reading inputs
        int numA = a.read();
        int numB = b.read();
        int numC = c.read();

        // Determining the largest number using if-else-if ladder
        int maxNum;
        if (numA >= numB) {
            if (numA >= numC)
                maxNum = numA;
            else
                maxNum = numC;
        } else {
            if (numB >= numC)
                maxNum = numB;
            else
                maxNum = numC;
        }

        // Writing the largest number to the output port
        largest.write(maxNum);
    }

    // Constructor to register the process
    SC_CTOR(FindLargest) {
        SC_METHOD(findLargestNumber);
        sensitive << a << b << c;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with FindLargest module
    sc_signal<int> sigA, sigB, sigC;
    sc_signal<int> sigLargest;

    // Instance of FindLargest module
    FindLargest findLargestInst;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing values
        sigA = 1;
        sigB = 2;
        sigC = 11;

        // Wait for one delta cycle to let the FindLargest module process
        wait(1, SC_NS);

        // Print the largest number
        cout << "The largest number among " << sigA.read() << ", " << sigB.read() << ", and " << sigC.read() << " is: " << sigLargest.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : findLargestInst("findLargestInst") {
        // Connecting signals to the FindLargest module ports
        findLargestInst.a(sigA); findLargestInst.b(sigB); findLargestInst.c(sigC);
        findLargestInst.largest(sigLargest);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
