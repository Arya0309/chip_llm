
#include <systemc.h>

SC_MODULE(BubbleSort) {
    sc_in_clk clk;          // Clock signal
    sc_in<bool> reset;      // Reset signal
    sc_vector<sc_in<int>> in; // Input vector
    sc_vector<sc_out<int>> out; // Output vector

    SC_CTOR(BubbleSort) : in("in", 5), out("out", 5) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();
        SC_METHOD(reset_process);
        sensitive << reset.pos();
    }

    void sort_process() {
        static int nums[5];
        static bool isSwapped;
        static int i = 0, j = 1;
        static bool initialized = false;

        if (!initialized) {
            for (int k = 0; k < 5; k++) {
                nums[k] = in[k].read();
            }
            initialized = true;
        }

        if (reset.read()) {
            i = 0;
            j = 1;
            initialized = false;
            for (int k = 0; k < 5; k++) {
                out[k].write(in[k].read());
            }
        } else {
            isSwapped = false;
            while (i < 5) {
                while (j < 5 - i) {
                    if (nums[j] < nums[j - 1]) {
                        int temp = nums[j];
                        nums[j] = nums[j - 1];
                        nums[j - 1] = temp;
                        isSwapped = true;
                    }
                    j++;
                }
                if (!isSwapped) break;
                i++;
                j = 1;
            }
            for (int k = 0; k < 5; k++) {
                out[k].write(nums[k]);
            }
        }
    }

    void reset_process() {
        if (reset.read()) {
            i = 0;
            j = 1;
            initialized = false;
            for (int k = 0; k < 5; k++) {
                out[k].write(in[k].read());
            }
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 10, SC_NS); // Create a clock
    sc_signal<bool> reset;              // Create a reset signal
    sc_vector<sc_signal<int>> in("in", 5); // Create input signals
    sc_vector<sc_signal<int>> out("out", 5); // Create output signals

    // Initialize input values
    in[0].write(1);
    in[1].write(12);
    in[2].write(6);
    in[3].write(8);
    in[4].write(10);

    // Instantiate the BubbleSort module
    BubbleSort sorter("sorter");
    sorter.clk(clock);
    sorter.reset(reset);
    for (int i = 0; i < 5; i++) {
        sorter.in[i](in[i]);
        sorter.out[i](out[i]);
    }

    // Apply reset
    reset.write(true);
    sc_start(10, SC_NS);
    reset.write(false);

    // Run simulation
    sc_start(100, SC_NS);

    // Print sorted array
    cout << "After sorting the array is: \n";
    for (int i = 0; i < 5; i++) {
        cout << out[i].read() << " ";
    }
    cout << "\n";

    return 0;
}
