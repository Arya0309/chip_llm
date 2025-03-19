
#include <systemc.h>
#include <vector>

// Module to perform merge sort
SC_MODULE(MergeSort) {
    // Input port for the vector to be sorted
    sc_in<bool> start; // Signal to start sorting
    sc_in<sc_uint<32>> vec_size; // Size of the vector
    sc_inout<sc_uint<32>> vec[6]; // Assuming maximum size of 6 for simplicity

    // Output port for the sorted vector
    sc_out<bool> done; // Signal indicating sorting is complete

    // Internal variables
    std::vector<int> internal_vec;
    bool sort_complete;

    // Process to perform merge sort
    void sort_process() {
        while (true) {
            wait(start.posedge_event()); // Wait for start signal

            // Copy input vector to internal vector
            internal_vec.resize(vec_size.read());
            for (int i = 0; i < vec_size.read(); ++i) {
                internal_vec[i] = vec[i]->read();
            }

            // Perform merge sort
            mergeSort(internal_vec, 0, vec_size.read() - 1);

            // Copy sorted vector back to output ports
            for (int i = 0; i < vec_size.read(); ++i) {
                vec[i]->write(internal_vec[i]);
            }

            // Signal completion
            done.write(true);
            wait(1, SC_NS); // Small delay before resetting done signal
            done.write(false);
        }
    }

    // Merge function
    void merge(std::vector<int>& vec, int left, int mid, int right) {
        int i, j, k;
        int n1 = mid - left + 1;
        int n2 = right - mid;
        std::vector<int> leftVec(n1), rightVec(n2);

        for (i = 0; i < n1; i++)
            leftVec[i] = vec[left + i];
        for (j = 0; j < n2; j++)
            rightVec[j] = vec[mid + 1 + j];

        i = 0;
        j = 0;
        k = left;
        while (i < n1 && j < n2) {
            if (leftVec[i] <= rightVec[j]) {
                vec[k] = leftVec[i];
                i++;
            } else {
                vec[k] = rightVec[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            vec[k] = leftVec[i];
            i++;
            k++;
        }

        while (j < n2) {
            vec[k] = rightVec[j];
            j++;
            k++;
        }
    }

    // MergeSort function
    void mergeSort(std::vector<int>& vec, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(vec, left, mid);
            mergeSort(vec, mid + 1, right);
            merge(vec, left, mid, right);
        }
    }

    // Constructor to register the process
    SC_CTOR(MergeSort) {
        SC_THREAD(sort_process);
        sensitive << start;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with MergeSort
    sc_signal<bool> start;
    sc_signal<sc_uint<32>> vec_size;
    sc_signal<sc_uint<32>> vec[6]; // Assuming maximum size of 6 for simplicity
    sc_signal<bool> done;

    // Instance of MergeSort
    MergeSort sorter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing vector
        vec_size = 6;
        vec[0] = 12;
        vec[1] = 11;
        vec[2] = 13;
        vec[3] = 5;
        vec[4] = 6;
        vec[5] = 7;

        // Start sorting
        start.write(true);
        wait(1, SC_NS);
        start.write(false);

        // Wait for sorting to complete
        wait(done.posedge_event());

        // Print the results
        cout << "Sorted vector: ";
        for (int i = 0; i < vec_size.read(); ++i) {
            cout << vec[i].read() << " ";
        }
        cout << endl;

        // End simulation
        sc_stop();
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : sorter("sorter") {
        // Connecting signals to the sorter ports
        sorter.start(start);
        sorter.vec_size(vec_size);
        for (int i = 0; i < 6; ++i) {
            sorter.vec[i](vec[i]);
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
