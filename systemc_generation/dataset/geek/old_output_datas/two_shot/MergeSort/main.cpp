
#include <systemc.h>
#include <vector>

// Module to perform merge sort
SC_MODULE(MergeSortModule) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> reset;        // Reset signal
    sc_in<bool> start;        // Start signal to begin sorting
    sc_out<bool> done;        // Done signal indicating completion
    sc_vector<sc_in<int>> input; // Input vector of integers
    sc_vector<sc_out<int>> output; // Output vector of integers

    std::vector<int> vec; // Internal vector to store and sort data
    bool sorting_done; // Flag to indicate sorting completion

    // Constructor
    SC_CTOR(MergeSortModule) : input("input", 6), output("output", 6) {
        // Sensitivity list for the process
        SC_METHOD(sort_process);
        sensitive << clk.posedge_event() << reset.posedge_event();
        dont_initialize();
    }

    // Method to perform merge sort
    void sort_process() {
        if (reset.read()) {
            sorting_done = false;
            done.write(false);
        } else if (start.read() && !sorting_done) {
            // Copy input data to internal vector
            vec.resize(input.size());
            for (size_t i = 0; i < input.size(); ++i) {
                vec[i] = input[i].read();
            }

            // Perform merge sort
            mergeSort(vec, 0, vec.size() - 1);

            // Write sorted data to output
            for (size_t i = 0; i < output.size(); ++i) {
                output[i].write(vec[i]);
            }
            sorting_done = true;
            done.write(true);
        }
    }

    // Merges two subarrays of vec.
    void merge(std::vector<int>& vec, int left, int mid, int right) {
        int i, j, k;
        int n1 = mid - left + 1;
        int n2 = right - mid;

        // Create temporary vectors
        std::vector<int> leftVec(n1), rightVec(n2);

        // Copy data to temporary vectors
        for (i = 0; i < n1; i++)
            leftVec[i] = vec[left + i];
        for (j = 0; j < n2; j++)
            rightVec[j] = vec[mid + 1 + j];

        // Merge the temporary vectors back into vec[left..right]
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

        // Copy the remaining elements of leftVec[], if any
        while (i < n1) {
            vec[k] = leftVec[i];
            i++;
            k++;
        }

        // Copy the remaining elements of rightVec[], if any
        while (j < n2) {
            vec[k] = rightVec[j];
            j++;
            k++;
        }
    }

    // The subarray to be sorted is in the index range [left..right]
    void mergeSort(std::vector<int>& vec, int left, int right) {
        if (left < right) {
            // Calculate the midpoint
            int mid = left + (right - left) / 2;

            // Sort first and second halves
            mergeSort(vec, left, mid);
            mergeSort(vec, mid + 1, right);

            // Merge the sorted halves
            merge(vec, left, mid, right);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;             // Clock signal
    sc_signal<bool> reset;   // Reset signal
    sc_signal<bool> start;   // Start signal to begin sorting
    sc_signal<bool> done;    // Done signal indicating completion
    sc_vector<sc_signal<int>> input; // Input vector of integers
    sc_vector<sc_signal<int>> output; // Output vector of integers

    MergeSortModule merge_sort_inst; // Instance of the MergeSortModule

    // Constructor
    SC_CTOR(Testbench)
    : clk("clk", 10, SC_NS, 0.5), input("input", 6), output("output", 6),
      merge_sort_inst("merge_sort_inst") {
        // Connect signals to ports
        merge_sort_inst.clk(clk);
        merge_sort_inst.reset(reset);
        merge_sort_inst.start(start);
        merge_sort_inst.done(done);
        for (size_t i = 0; i < input.size(); ++i) {
            merge_sort_inst.input[i](input[i]);
            merge_sort_inst.output[i](output[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Reset the system
        reset.write(true);
        wait(2, SC_NS);
        reset.write(false);
        wait(2, SC_NS);

        // Initialize input data
        std::vector<int> vec = {12, 11, 13, 5, 6, 7};
        for (size_t i = 0; i < input.size(); ++i) {
            input[i].write(vec[i]);
        }

        // Start sorting
        start.write(true);
        wait(1, SC_NS);
        start.write(false);

        // Wait for sorting to complete
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Read and print sorted output
        for (size_t i = 0; i < output.size(); ++i) {
            std::cout << output[i].read() << " ";
        }
        std::cout << std::endl;

        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
