
#include <systemc.h>

SC_MODULE(Sorter) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_out<sc_uint<32>> sorted_array[5];

    SC_CTOR(Sorter) {
        SC_METHOD(sort_array);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(print_sorted_array);
        sensitive << clk.pos();
        dont_initialize();
    }

private:
    int nums[5] = {1, 12, 6, 8, 10};
    int size_nums = 5;
    bool isSwapped;

    void sort_array() {
        if (reset.read()) {
            for (int i = 0; i < size_nums; i++) {
                nums[i] = {1, 12, 6, 8, 10}[i];
            }
        } else {
            isSwapped = false;
            for (int j = 1; j < size_nums; j++) {
                if (nums[j] < nums[j - 1]) {
                    swapNums(nums, j, j - 1);
                    isSwapped = true;
                }
            }
            if (!isSwapped) {
                sc_stop();
            }
        }
    }

    void print_sorted_array() {
        if (reset.read()) {
            cout << "Before sorting the array is: \n";
            for (int i = 0; i < size_nums; i++) {
                cout << nums[i] << " ";
            }
            cout << "\n\n";
        } else {
            cout << "After sorting the array is: \n";
            for (int i = 0; i < size_nums; i++) {
                cout << nums[i] << " ";
                sorted_array[i].write(nums[i]);
            }
            cout << "\n";
        }
    }

    void swapNums(int nums[], int first, int second) {
        int curr = nums[first];
        nums[first] = nums[second];
        nums[second] = curr;
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS); // 10 ns period
    sc_signal<bool> reset;

    Sorter sorter("sorter");
    sorter.clk(clk);
    sorter.reset(reset);

    reset.write(true);
    sc_start(20, SC_NS); // Apply reset for 20 ns
    reset.write(false);
    sc_start(100, SC_NS); // Run for 100 ns

    return 0;
}
