
#include <systemc.h>

SC_MODULE(MergeArrays) {
    sc_in<int> arr1[4];
    sc_in<int> arr2[4];
    sc_out<int> arr3[8];
    sc_in<bool> clk;
    sc_in<bool> rst;

    SC_CTOR(MergeArrays) {
        SC_METHOD(merge_process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void merge_process() {
        if (rst.read()) {
            // Reset state if needed
            for (int i = 0; i < 8; ++i) {
                arr3[i].write(0);
            }
        } else {
            int temp_arr1[4];
            int temp_arr2[4];
            int temp_arr3[8];

            // Copy input values to local arrays
            for (int i = 0; i < 4; ++i) {
                temp_arr1[i] = arr1[i].read();
                temp_arr2[i] = arr2[i].read();
            }

            // Merge arrays without sorting (assuming inputs are sorted)
            int i = 0, j = 0, k = 0;
            while (i < 4) {
                temp_arr3[k++] = temp_arr1[i++];
            }
            while (j < 4) {
                temp_arr3[k++] = temp_arr2[j++];
            }

            // Sort the merged array
            std::sort(temp_arr3, temp_arr3 + 8);

            // Write results to output ports
            for (int i = 0; i < 8; ++i) {
                arr3[i].write(temp_arr3[i]);
            }
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<int> arr1[4];
    sc_signal<int> arr2[4];
    sc_signal<int> arr3[8];
    sc_clock clk("clk", 10, SC_NS, 0.5, 0, SC_NS, true);
    sc_signal<bool> rst;

    MergeArrays merge_arrays("merge_arrays");
    for (int i = 0; i < 4; ++i) {
        merge_arrays.arr1[i](arr1[i]);
        merge_arrays.arr2[i](arr2[i]);
        merge_arrays.arr3[i](arr3[i]);
    }
    merge_arrays.clk(clk);
    merge_arrays.rst(rst);

    // Initialize inputs
    arr1[0].write(1);
    arr1[1].write(3);
    arr1[2].write(5);
    arr1[3].write(7);

    arr2[0].write(2);
    arr2[1].write(4);
    arr2[2].write(6);
    arr2[3].write(8);

    rst.write(true);
    sc_start(10, SC_NS);
    rst.write(false);
    sc_start(10, SC_NS);

    // Output results
    cout << "Array after merging" << endl;
    for (int i = 0; i < 8; ++i) {
        cout << arr3[i].read() << " ";
    }
    cout << endl;

    return 0;
}
