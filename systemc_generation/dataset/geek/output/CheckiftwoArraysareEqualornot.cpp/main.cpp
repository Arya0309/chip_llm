
#include <systemc.h>

SC_MODULE(ArrayComparator) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_out<bool> result;

    int arr1[5];
    int arr2[5];
    int N;
    int M;
    bool equal;

    SC_CTOR(ArrayComparator) : N(5), M(5) {
        SC_METHOD(compare_arrays);
        sensitive << clk.posedge_event();
        dont_initialize();

        SC_METHOD(initialize_arrays);
        sensitive << rst.posedge_event();
        dont_initialize();
    }

    void initialize_arrays() {
        arr1[0] = 1; arr1[1] = 2; arr1[2] = 3; arr1[3] = 4; arr1[4] = 5;
        arr2[0] = 5; arr2[1] = 4; arr2[2] = 3; arr2[3] = 2; arr2[4] = 1;
        equal = false;
    }

    void compare_arrays() {
        if (rst.read()) {
            equal = false;
            return;
        }

        // Check lengths of arrays
        if (N != M) {
            equal = false;
            return;
        }

        // Sort both arrays
        std::sort(arr1, arr1 + N);
        std::sort(arr2, arr2 + M);

        // Compare elements of sorted arrays
        equal = true;
        for (int i = 0; i < N; i++) {
            if (arr1[i] != arr2[i]) {
                equal = false;
                break;
            }
        }

        result.write(equal);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<bool> clk;
    sc_signal<bool> rst;
    sc_signal<bool> result;

    ArrayComparator comparator("comparator");
    comparator.clk(clk);
    comparator.rst(rst);
    comparator.result(result);

    // Initialize clock and reset
    sc_start(0, SC_NS);
    rst.write(true);
    sc_start(1, SC_NS);
    rst.write(false);
    sc_start(1, SC_NS);

    // Simulate for some time
    sc_start(10, SC_NS);

    // Print result
    std::cout << "Arrays are " << (result.read() ? "Equal" : "Not Equal") << std::endl;

    return 0;
}
