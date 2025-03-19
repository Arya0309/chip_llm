
#include <systemc.h>

SC_MODULE(BubbleSortModule) {
    sc_in_clk clk;
    sc_in<bool> reset;
    sc_out<sc_uint<8>> sorted_array[4];

    SC_CTOR(BubbleSortModule) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(output_process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void sort_process() {
        static int arr[4] = {6, 0, 3, 5};
        static int n = 4;
        static bool done = false;

        if (reset.read()) {
            done = false;
        } else if (!done) {
            for (int i = 0; i < n - 1; i++) {
                for (int j = 0; j < n - i - 1; j++) {
                    if (arr[j] > arr[j + 1]) {
                        int temp = arr[j];
                        arr[j] = arr[j + 1];
                        arr[j + 1] = temp;
                    }
                }
            }
            done = true;
        }
    }

    void output_process() {
        static int arr[4] = {6, 0, 3, 5};
        static int n = 4;
        static bool output_done = false;

        if (reset.read()) {
            output_done = false;
        } else if (!output_done) {
            for (int i = 0; i < n; i++) {
                sorted_array[i].write(arr[i]);
            }
            output_done = true;
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset;
    sc_signal<sc_uint<8>> sorted_array[4];

    BubbleSortModule bubble_sort_module("bubble_sort_module");
    bubble_sort_module.clk(clk);
    bubble_sort_module.reset(reset);
    for (int i = 0; i < 4; i++) {
        bubble_sort_module.sorted_array[i](sorted_array[i]);
    }

    reset.write(true);
    sc_start(10, SC_NS); // Reset period
    reset.write(false);
    sc_start(100, SC_NS); // Sorting period

    // Print sorted array
    std::cout << "Sorted array: ";
    for (int i = 0; i < 4; i++) {
        std::cout << sorted_array[i].read() << " ";
    }
    std::cout << std::endl;

    return 0;
}
