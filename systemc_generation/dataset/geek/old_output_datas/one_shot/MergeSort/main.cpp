
#include <systemc.h>
#include <vector>

// Module to perform merge sort
SC_MODULE(MergeSort) {
    sc_in<bool> clk; // Clock signal
    sc_in<bool> reset; // Reset signal
    sc_vector<sc_in<int>> input; // Input vector
    sc_vector<sc_out<int>> output; // Output vector
    sc_in<bool> start; // Start signal to begin sorting
    sc_out<bool> done; // Done signal indicating sorting completion

    // Internal state
    std::vector<int> vec;
    bool sorting;

    // Constructor
    SC_CTOR(MergeSort) {
        // Register processes
        SC_METHOD(sort_process);
        sensitive << clk.posedge_event() << reset.posedge_event();
        dont_initialize();

        // Initialize internal state
        sorting = false;
    }

    // Process to handle sorting
    void sort_process() {
        if (reset.read()) {
            sorting = false;
            done.write(false);
        } else if (start.read() && !sorting) {
            // Copy input to internal vector
            vec.resize(input.size());
            for (size_t i = 0; i < input.size(); ++i) {
                vec[i] = input[i].read();
            }
            sorting = true;
        } else if (sorting) {
            // Perform merge sort
            mergeSort(vec, 0, vec.size() - 1);
            sorting = false;
            done.write(true);

            // Write sorted vector to output
            for (size_t i = 0; i < output.size(); ++i) {
                output[i].write(vec[i]);
            }
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

    // Merge Sort function
    void mergeSort(std::vector<int>& vec, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(vec, left, mid);
            mergeSort(vec, mid + 1, right);
            merge(vec, left, mid, right);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk; // Clock signal
    sc_signal<bool> reset; // Reset signal
    sc_vector<sc_signal<int>> input; // Input vector
    sc_vector<sc_signal<int>> output; // Output vector
    sc_signal<bool> start; // Start signal to begin sorting
    sc_signal<bool> done; // Done signal indicating sorting completion

    MergeSort merge_sort_inst; // Instance of the MergeSort module

    // Constructor
    SC_CTOR(Testbench) : merge_sort_inst("merge_sort"), input(6), output(6) {
        // Connect signals to ports
        merge_sort_inst.clk(clk);
        merge_sort_inst.reset(reset);
        merge_sort_inst.start(start);
        merge_sort_inst.done(done);
        for (size_t i = 0; i < input.size(); ++i) {
            merge_sort_inst.input[i](input[i]);
            merge_sort_inst.output[i](output[i]);
        }

        // Initialize signals
        reset.write(true);
        sc_start(1, SC_NS);
        reset.write(false);

        // Initialize input vector
        input[0].write(12);
        input[1].write(11);
        input[2].write(13);
        input[3].write(5);
        input[4].write(6);
        input[5].write(7);

        // Start sorting
        start.write(true);
        sc_start(1, SC_NS);
        start.write(false);

        // Wait for sorting to complete
        while (!done.read()) {
            sc_start(1, SC_NS);
        }

        // Print sorted output
        cout << "Sorted vector: ";
        for (size_t i = 0; i < output.size(); ++i) {
            cout << output[i].read() << " ";
        }
        cout << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
