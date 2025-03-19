
#include <systemc.h>

// Module to sort an array using Bubble Sort
SC_MODULE(BubbleSortModule) {
    // Input and output ports
    sc_in<bool> clk; // Clock signal
    sc_in<bool> reset; // Reset signal
    sc_vector<sc_in<int>> input; // Input vector for the array
    sc_vector<sc_out<int>> output; // Output vector for the sorted array
    sc_out<bool> done; // Signal indicating sorting completion

    // Internal variables
    int *array;
    int length;
    int i, j;
    bool isSwapped;

    // Process to perform bubble sort
    void bubbleSort() {
        if (reset.read()) {
            // Initialize variables
            i = 0;
            j = 1;
            isSwapped = false;
            done.write(false);
        } else if (clk.posedge()) {
            if (i < length - 1) {
                if (j < length - i) {
                    if (array[j] < array[j - 1]) {
                        // Swap elements
                        int temp = array[j];
                        array[j] = array[j - 1];
                        array[j - 1] = temp;
                        isSwapped = true;
                    }
                    j++;
                } else {
                    if (!isSwapped) {
                        done.write(true);
                    }
                    i++;
                    j = 1;
                    isSwapped = false;
                }
            }
        }
    }

    // Constructor to initialize the module
    SC_CTOR(BubbleSortModule) : length(input.size()) {
        array = new int[length];
        done.initialize(false);

        SC_METHOD(bubbleSort);
        sensitive << clk.pos() << reset.pos();
    }

    // Destructor to clean up
    ~BubbleSortModule() {
        delete[] array;
    }

    // Method to load input data into internal array
    void loadData() {
        for (int k = 0; k < length; k++) {
            array[k] = input[k].read();
        }
    }

    // Method to write sorted data to output ports
    void writeData() {
        for (int k = 0; k < length; k++) {
            output[k].write(array[k]);
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with BubbleSortModule
    sc_clock clk;
    sc_signal<bool> reset;
    sc_vector<sc_signal<int>> input;
    sc_vector<sc_signal<int>> output;
    sc_signal<bool> done;

    // Instance of BubbleSortModule
    BubbleSortModule sorter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing input array
        input[0].write(1);
        input[1].write(12);
        input[2].write(6);
        input[3].write(8);
        input[4].write(10);

        // Reset the sorter
        reset.write(true);
        wait(1, SC_NS);
        reset.write(false);
        sorter.loadData();

        // Wait for sorting to complete
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Write sorted data to output ports
        sorter.writeData();

        // Print the results
        cout << "Sorted array is: ";
        for (int i = 0; i < input.size(); i++) {
            cout << output[i].read() << " ";
        }
        cout << endl;

        sc_stop(); // Stop simulation
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : clk("clk", 1, SC_NS), sorter("sorter", input.size()) {
        // Connecting signals to the sorter ports
        sorter.clk(clk);
        sorter.reset(reset);
        sorter.done(done);

        for (int i = 0; i < input.size(); i++) {
            sorter.input[i](input[i]);
            sorter.output[i](output[i]);
        }

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    // Create a testbench instance with an array size of 5
    Testbench tb("tb", 5);
    sc_start(); // Start simulation
    return 0;
}
