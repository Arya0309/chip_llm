
#include <systemc.h>

SC_MODULE(SelectionSorter) {
    sc_vector<sc_signal<int>> input_arr;
    sc_signal<int> sorted_arr[5];
    sc_in<bool> clk;
    sc_out<bool> done;

    SC_CTOR(SelectionSorter) : input_arr("input_arr", 5) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();
    }

    void sort_process() {
        static bool initialized = false;
        static int arr[5];
        static int i = 0, j = 0, min_idx = 0;
        static bool sorting = false;

        if (!initialized) {
            // Initialize array from input signals
            for (int k = 0; k < 5; ++k) {
                arr[k] = input_arr[k].read();
            }
            initialized = true;
            sorting = true;
        }

        if (sorting) {
            if (i < 4) {
                if (j < 5) {
                    if (arr[j] < arr[min_idx]) {
                        min_idx = j;
                    }
                    j++;
                } else {
                    // Swap the found minimum element with the first element
                    int temp = arr[min_idx];
                    arr[min_idx] = arr[i];
                    arr[i] = temp;

                    // Move to the next element
                    i++;
                    j = i + 1;
                    min_idx = i;
                }
            } else {
                // Sorting is complete
                sorting = false;
                done.write(true);
                // Write sorted array to output signals
                for (int k = 0; k < 5; ++k) {
                    sorted_arr[k].write(arr[k]);
                }
            }
        }
    }
};

SC_MODULE(TopLevel) {
    SelectionSorter sorter;
    sc_signal<bool> clk;
    sc_signal<bool> done;

    SC_CTOR(TopLevel) : sorter("sorter") {
        sorter.clk(clk);
        sorter.done(done);

        SC_THREAD(clock_gen);
        SC_THREAD(check_done);
    }

    void clock_gen() {
        while (true) {
            clk = 0;
            wait(10, SC_NS);
            clk = 1;
            wait(10, SC_NS);
        }
    }

    void check_done() {
        wait(done.posedge_event());
        cout << "Sorted array: ";
        for (int i = 0; i < 5; ++i) {
            cout << sorter.sorted_arr[i].read() << " ";
        }
        cout << endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    TopLevel top("top");
    sc_start();
    return 0;
}
