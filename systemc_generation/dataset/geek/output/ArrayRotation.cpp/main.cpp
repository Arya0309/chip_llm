
#include <systemc.h>

SC_MODULE(ArrayRotator) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_out<bool> done;

    SC_CTOR(ArrayRotator) {
        SC_METHOD(rotate_array);
        sensitive << clk.pos();

        SC_METHOD(print_array);
        sensitive << done;
    }

    void rotate_array() {
        static bool initialized = false;
        static int arr[7] = {1, 2, 3, 4, 5, 6, 7};
        static int temp[7];
        static int d = 2;
        static int n = 7;
        static int k = 0;
        static int phase = 0;

        if (!reset.read()) {
            initialized = false;
            k = 0;
            phase = 0;
            done.write(false);
        } else if (!initialized) {
            // First loop: Copying elements to Temp Array
            if (k < n - d) {
                temp[k] = arr[k + d];
                k++;
            } else {
                phase = 1;
                k = 0;
            }
            initialized = true;
        } else if (phase == 1) {
            // Second loop: Copying Remaining Elements to Temp Array
            if (k < d) {
                temp[k + n - d] = arr[k];
                k++;
            } else {
                phase = 2;
                k = 0;
            }
        } else if (phase == 2) {
            // Third loop: Copying Back to Original Array
            if (k < n) {
                arr[k] = temp[k];
                k++;
            } else {
                phase = 3;
                done.write(true);
            }
        }
    }

    void print_array() {
        if (done.read()) {
            for (int i = 0; i < 7; i++) {
                cout << arr[i] << " ";
            }
            cout << endl;
        }
    }

private:
    int arr[7];
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset;
    sc_signal<bool> done;

    ArrayRotator rotator("rotator");
    rotator.clk(clk);
    rotator.reset(reset);
    rotator.done(done);

    reset.write(false);
    sc_start(10, SC_NS); // Reset period
    reset.write(true);
    sc_start(100, SC_NS); // Simulation period

    return 0;
}
