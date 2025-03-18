#include <systemc.h>
#include <vector>
#include <cassert>

SC_MODULE(BubbleSortModule) {
    sc_in_clk clk;  // Clock signal
    sc_in<bool> rst;  // Reset signal
    sc_vector<sc_in<int>> input;  // Input vector
    sc_vector<sc_out<int>> output;  // Output vector
    sc_out<bool> done;  // Signal indicating sorting is complete

    SC_CTOR(BubbleSortModule) : input("input", 10), output("output", 10) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void sort_process() {
        if (rst.read()) {
            // Reset state
            sorted = false;
            current_index = 0;
            inner_index = 0;
            swapped = false;
            for (int i = 0; i < N; ++i) {
                temp_storage[i] = 0;
            }
            done.write(false);
        } else {
            if (!sorted) {
                if (inner_index < N - current_index - 1) {
                    if (temp_storage[inner_index] > temp_storage[inner_index + 1]) {
                        // Swap elements
                        int temp = temp_storage[inner_index];
                        temp_storage[inner_index] = temp_storage[inner_index + 1];
                        temp_storage[inner_index + 1] = temp;
                        swapped = true;
                    }
                    inner_index++;
                } else {
                    if (!swapped) {
                        sorted = true;
                        done.write(true);
                    } else {
                        swapped = false;
                        current_index++;
                        inner_index = 0;
                    }
                }
            } else {
                // Copy sorted array to output
                for (int i = 0; i < N; ++i) {
                    output[i].write(temp_storage[i]);
                }
            }
        }
    }

private:
    static const int N = 10;  // Maximum array size (constrained to 10 for simplicity)
    int temp_storage[N];  // Temporary storage for sorting
    bool sorted;
    int current_index;
    int inner_index;
    bool swapped;
};

SC_MODULE(TestBench) {
    sc_clock clk;
    sc_signal<bool> rst;
    sc_vector<sc_signal<int>> input;
    sc_vector<sc_signal<int>> output;
    sc_signal<bool> done;

    SC_CTOR(TestBench) : clk("clk", 10, SC_NS, 0.5), input("input", 10), output("output", 10) {
        BubbleSortModule sorter("sorter");
        sorter.clk(clk);
        sorter.rst(rst);
        for (int i = 0; i < 10; ++i) {
            sorter.input[i](input[i]);
            sorter.output[i](output[i]);
        }
        sorter.done(done);

        SC_THREAD(test_process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void test_process() {
        rst.write(true);
        wait(2);  // Wait for 2 clock cycles to reset
        rst.write(false);

        // Test case 1: Unsorted array
        std::vector<int> testArray1 = {64, 34, 25, 12, 22, 11, 90};
        for (int i = 0; i < 10; ++i) {
            input[i].write(i < 7 ? testArray1[i] : 0);
        }
        wait_until(done.posedge_event());
        assert(isSorted(output));

        // Test case 2: Already sorted array
        std::vector<int> testArray2 = {1, 2, 3, 4, 5};
        for (int i = 0; i < 10; ++i) {
            input[i].write(i < 5 ? testArray2[i] : 0);
        }
        wait_until(done.posedge_event());
        assert(isSorted(output));

        // Test case 3: Reverse sorted array
        std::vector<int> testArray3 = {9, 8, 7, 6, 5};
        for (int i = 0; i < 10; ++i) {
            input[i].write(i < 5 ? testArray3[i] : 0);
        }
        wait_until(done.posedge_event());
        assert(isSorted(output));

        // Test case 4: Array with duplicate values
        std::vector<int> testArray4 = {3, 3, 2, 1, 2};
        for (int i = 0; i < 10; ++i) {
            input[i].write(i < 5 ? testArray4[i] : 0);
        }
        wait_until(done.posedge_event());
        assert(isSorted(output));

        std::cout << "All bubble sort tests passed!" << std::endl;
        sc_stop();
    }

    bool isSorted(sc_vector<sc_signal<int>>& arr) {
        for (size_t i = 1; i < arr.size(); ++i) {
            if (arr[i - 1].read() > arr[i].read())
                return false;
        }
        return true;
    }
};

int sc_main(int argc, char* argv[]) {
    TestBench tb("tb");
    sc_start();
    return 0;
}