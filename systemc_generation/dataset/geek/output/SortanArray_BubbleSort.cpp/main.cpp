
#include <systemc.h>
#include <iostream>
using namespace std;

// Module to perform Bubble Sort
SC_MODULE(BubbleSortModule) {
    // Input and output ports
    sc_vector<sc_in<int>> input;
    sc_vector<sc_out<int>> output;
    sc_in<bool> clk;
    sc_in<bool> reset;

    // Internal storage for the array
    int* arr;
    int n;
    int step;
    int i;
    int j;

    // Process to perform Bubble Sort
    void bubbleSortProcess() {
        if (reset.read()) {
            // Initialize variables
            step = 0;
            i = 0;
            j = 0;
            for (int k = 0; k < n; ++k)
                arr[k] = input[k].read();
        } else if (clk.read()) {
            switch (step) {
                case 0: // Outer loop start
                    if (i < n - 1) {
                        j = 0;
                        step = 1;
                    } else {
                        step = 3; // Sorting complete
                    }
                    break;
                case 1: // Inner loop start
                    if (j < n - i - 1) {
                        if (arr[j] > arr[j + 1]) {
                            // Swap elements
                            int temp = arr[j];
                            arr[j] = arr[j + 1];
                            arr[j + 1] = temp;
                        }
                        j++;
                    } else {
                        i++;
                        step = 0;
                    }
                    break;
                case 3: // Write output
                    for (int k = 0; k < n; ++k)
                        output[k].write(arr[k]);
                    break;
            }
        }
    }

    // Constructor
    SC_CTOR(BubbleSortModule) : step(0), i(0), j(0) {
        SC_METHOD(bubbleSortProcess);
        sensitive << clk.pos() << reset.pos();
    }

    // Initialization
    void initialize(int size) {
        n = size;
        arr = new int[n];
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with BubbleSortModule
    sc_vector<sc_signal<int>> input;
    sc_vector<sc_signal<int>> output;
    sc_signal<bool> clk;
    sc_signal<bool> reset;

    // Instance of BubbleSortModule
    BubbleSortModule sorter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize inputs
        input[0].write(6);
        input[1].write(0);
        input[2].write(3);
        input[3].write(5);

        // Reset signal
        reset.write(true);
        wait(1, SC_NS);
        reset.write(false);

        // Clock signal
        while (true) {
            clk.write(false);
            wait(1, SC_NS);
            clk.write(true);
            wait(1, SC_NS);

            // Check if sorting is done
            if (sorter.step == 3) {
                cout << "Sorted array: ";
                for (int k = 0; k < 4; ++k)
                    cout << output[k].read() << " ";
                cout << endl;
                sc_stop();
            }
        }
    }

    // Constructor
    SC_CTOR(Testbench) : sorter("sorter", 4) {
        // Connecting signals to the sorter ports
        for (int i = 0; i < 4; ++i) {
            sorter.input[i](input[i]);
            sorter.output[i](output[i]);
        }
        sorter.clk(clk);
        sorter.reset(reset);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
