
#include <systemc.h>

SC_MODULE(RemoveOccurrences) {
    sc_in<sc_int<32>> target;
    sc_vector<sc_in<sc_int<32>>> arr;
    sc_out<bool> done;
    sc_fifo<sc_int<32>> result;

    SC_CTOR(RemoveOccurrences) : arr("arr", 11), done("done"), result("result") {
        SC_METHOD(remove_occurrences);
        sensitive << target;
    }

    void remove_occurrences() {
        int cnt = 0;
        int n = arr.size();

        // Counting all occurrences of target element
        for (int i = 0; i < n; i++) {
            if (arr[i].read() == target.read()) {
                cnt++;
            }
        }

        // Creating new array of size = original size - no. of target element
        int new_arr[n - cnt];
        int ind = 0;
        for (int i = 0; i < n; i++) {
            if (arr[i].read() != target.read()) {
                new_arr[ind] = arr[i].read();
                ind++;
            }
        }

        // Writing the new array to the result fifo
        for (int i = 0; i < ind; i++) {
            result.write(new_arr[i]);
        }

        done.write(true);
    }
};

int sc_main(int argc, char* argv[]) {
    RemoveOccurrences remove_occurrences_inst("remove_occurrences_inst");

    // Initialize target
    sc_signal<sc_int<32>> target;
    target.write(3);

    // Initialize array
    sc_vector<sc_signal<sc_int<32>>> arr("arr", 11);
    arr[0].write(1);
    arr[1].write(4);
    arr[2].write(3);
    arr[3].write(6);
    arr[4].write(8);
    arr[5].write(3);
    arr[6].write(9);
    arr[7].write(10);
    arr[8].write(3);
    arr[9].write(3);
    arr[10].write(7);

    // Connect signals
    remove_occurrences_inst.target(target);
    for (int i = 0; i < 11; i++) {
        remove_occurrences_inst.arr[i](arr[i]);
    }

    // Output FIFO
    sc_fifo<sc_int<32>> result("result");
    remove_occurrences_inst.result(result);

    // Done signal
    sc_signal<bool> done;
    remove_occurrences_inst.done(done);

    // Simulation loop
    sc_start();

    // Print results
    while (!result.empty()) {
        std::cout << result.read() << " ";
    }
    std::cout << std::endl;

    return 0;
}
