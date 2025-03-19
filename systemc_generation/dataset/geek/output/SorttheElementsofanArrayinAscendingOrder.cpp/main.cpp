
#include <systemc.h>

// Module to perform Bubble Sort
SC_MODULE(BubbleSort) {
    // Input ports
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> reset;        // Reset signal
    sc_in<sc_uint<32>> len;   // Length of the array
    sc_in<sc_uint<32>> dataIn[10]; // Input data array (assuming max length 10)

    // Output ports
    sc_out<sc_uint<32>> dataOut[10]; // Output data array
    sc_out<bool> done;              // Sorting complete signal

    // Internal variables
    sc_uint<32> nums[10];
    sc_uint<32> i, j;
    bool isSwapped;

    // Process to perform sorting
    void bubbleSort() {
        if (reset.read()) {
            i = 0;
            j = 1;
            isSwapped = false;
            for (int k = 0; k < len.read(); k++) {
                nums[k] = dataIn[k].read();
                dataOut[k].write(0);
            }
            done.write(false);
        } else if (clk.event() && !done.read()) {
            if (i < len.read()) {
                if (j < len.read() - i) {
                    if (nums[j] < nums[j - 1]) {
                        sc_uint<32> temp = nums[j];
                        nums[j] = nums[j - 1];
                        nums[j - 1] = temp;
                        isSwapped = true;
                    }
                    j++;
                } else {
                    if (!isSwapped) {
                        done.write(true);
                    } else {
                        isSwapped = false;
                        i++;
                        j = 1;
                    }
                }
            } else {
                for (int k = 0; k < len.read(); k++) {
                    dataOut[k].write(nums[k]);
                }
                done.write(true);
            }
        }
    }

    // Constructor to register the process
    SC_CTOR(BubbleSort) {
        SC_METHOD(bubbleSort);
        sensitive << clk.pos() << reset.pos();
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with BubbleSort
    sc_signal<bool> clk, reset;
    sc_signal<sc_uint<32>> len;
    sc_signal<sc_uint<32>> dataIn[10];
    sc_signal<sc_uint<32>> dataOut[10];
    sc_signal<bool> done;

    // Instance of BubbleSort
    BubbleSort sorter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize clock and reset
        reset = 1;
        wait(1, SC_NS);
        reset = 0;
        wait(1, SC_NS);

        // Initialize array and length
        len = 5;
        dataIn[0] = 1;
        dataIn[1] = 12;
        dataIn[2] = 6;
        dataIn[3] = 8;
        dataIn[4] = 10;

        // Run sorting process
        while (!done.read()) {
            clk = 0;
            wait(1, SC_NS);
            clk = 1;
            wait(1, SC_NS);
        }

        // Print the sorted array
        cout << "Sorted array: ";
        for (int i = 0; i < len.read(); i++) {
            cout << dataOut[i].read() << " ";
        }
        cout << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : sorter("sorter") {
        // Connecting signals to the sorter ports
        sorter.clk(clk);
        sorter.reset(reset);
        sorter.len(len);
        for (int i = 0; i < 10; i++) {
            sorter.dataIn[i](dataIn[i]);
            sorter.dataOut[i](dataOut[i]);
        }
        sorter.done(done);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
