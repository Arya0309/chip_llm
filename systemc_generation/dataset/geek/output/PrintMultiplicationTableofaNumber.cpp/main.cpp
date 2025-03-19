
#include <systemc.h>

SC_MODULE(MultiplicationTable) {
    SC_CTOR(MultiplicationTable) {
        SC_THREAD(print_table);
    }

    void print_table() {
        int n = 5; // Number for which the multiplication table is printed
        for (int i = 1; i <= 10; ++i) {
            cout << n << " * " << i << " = " << n * i << endl;
        }
        sc_stop(); // Stop the simulation after printing the table
    }
};

int sc_main(int argc, char* argv[]) {
    MultiplicationTable mt("MultiplicationTable");
    sc_start();
    return 0;
}
