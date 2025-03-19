
#include <systemc.h>

// Module to copy and reverse the array
SC_MODULE(ArrayCopier) {
    SC_CTOR(ArrayCopier) {
        SC_THREAD(copy_and_reverse);
    }

    void copy_and_reverse() {
        // Define the original array
        int original_arr[5] = {1, 2, 3, 4, 5};
        int len = sizeof(original_arr) / sizeof(original_arr[0]);

        // Declare the copied array
        int copied_arr[5];

        // Copy elements in reverse order
        for (int i = 0; i < len; i++) {
            copied_arr[i] = original_arr[len - i - 1];
        }

        // Print the original array
        cout << "Original array: ";
        for (int i = 0; i < len; i++) {
            cout << original_arr[i] << " ";
        }
        cout << endl;

        // Print the copied array
        cout << "Resultant array: ";
        for (int i = 0; i < len; i++) {
            cout << copied_arr[i] << " ";
        }
        cout << endl;

        // End the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    ArrayCopier copier("copier");
    sc_start();
    return 0;
}
