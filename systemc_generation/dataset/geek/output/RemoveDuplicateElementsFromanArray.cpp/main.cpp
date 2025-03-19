
#include <systemc.h>

SC_MODULE(RemoveDuplicatesModule) {
    sc_in<sc_uint<32>> clk;
    sc_in<bool> reset;
    sc_in<sc_uint<32>> arr_size;
    sc_inout<sc_uint<32>>* arr;
    sc_out<sc_uint<32>> new_size;

    SC_CTOR(RemoveDuplicatesModule) : arr(new sc_inout<sc_uint<32>>[arr_size.read()]) {
        SC_METHOD(remove_duplicates);
        sensitive << clk.pos();
        dont_initialize();
    }

    ~RemoveDuplicatesModule() {
        delete[] arr;
    }

    void remove_duplicates() {
        if (reset.read()) {
            new_size.write(0);
        } else {
            int n = arr_size.read();
            if (n == 0 || n == 1) {
                new_size.write(n);
                return;
            }

            sc_uint<32> temp[n];
            int j = 0;

            for (int i = 0; i < n - 1; i++) {
                if (arr[i].read() != arr[i + 1].read()) {
                    temp[j++] = arr[i].read();
                }
            }

            temp[j++] = arr[n - 1].read();

            for (int i = 0; i < j; i++) {
                arr[i].write(temp[i]);
            }

            new_size.write(j);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<sc_uint<32>> clk;
    sc_signal<bool> reset;
    sc_signal<sc_uint<32>> arr_size;
    sc_signal<sc_uint<32>> new_size;

    const int array_size = 9;
    sc_signal<sc_uint<32>> arr[array_size];

    RemoveDuplicatesModule remove_duplicates_inst("remove_duplicates_inst");
    remove_duplicates_inst.clk(clk);
    remove_duplicates_inst.reset(reset);
    remove_duplicates_inst.arr_size(arr_size);
    remove_duplicates_inst.new_size(new_size);

    for (int i = 0; i < array_size; i++) {
        remove_duplicates_inst.arr[i](arr[i]);
    }

    // Initialize array values
    arr[0].write(1);
    arr[1].write(2);
    arr[2].write(2);
    arr[3].write(3);
    arr[4].write(4);
    arr[5].write(4);
    arr[6].write(4);
    arr[7].write(5);
    arr[8].write(5);

    arr_size.write(array_size);
    reset.write(true);
    clk.write(false);
    sc_start(1, SC_NS);
    reset.write(false);
    clk.write(true);
    sc_start(1, SC_NS);

    cout << "Updated array: ";
    for (int i = 0; i < new_size.read(); i++) {
        cout << arr[i].read() << " ";
    }
    cout << endl;

    sc_stop();
    return 0;
}
