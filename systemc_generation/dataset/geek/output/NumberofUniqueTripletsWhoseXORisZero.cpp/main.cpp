
#include <systemc.h>
#include <unordered_set>

SC_MODULE(TripletCounter) {
    sc_in<sc_int<32>> arr[6]; // Input array of integers
    sc_out<sc_int<32>> result; // Output count of triplets

    SC_CTOR(TripletCounter) {
        SC_METHOD(count_triplets);
        sensitive << arr[0] << arr[1] << arr[2] << arr[3] << arr[4] << arr[5];
    }

    void count_triplets() {
        int a[6];
        for (int i = 0; i < 6; ++i) {
            a[i] = arr[i].read();
        }

        // To store values that are present
        std::unordered_set<int> s;
        for (int i = 0; i < 6; i++) {
            s.insert(a[i]);
        }

        // Stores the count of unique triplets
        int count = 0;

        // Traverse for all i, j pairs such that j>i
        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 6; j++) {
                // XOR of a[i] and a[j]
                int xr = a[i] ^ a[j];

                // If XR of two numbers is present,
                // then increase the count
                if (s.find(xr) != s.end() && xr != a[i] && xr != a[j]) {
                    count++;
                }
            }
        }

        // Returns answer
        result.write(count / 3);
    }
};

int sc_main(int argc, char* argv[]) {
    TripletCounter triplet_counter("triplet_counter");

    sc_signal<sc_int<32>> a0, a1, a2, a3, a4, a5, res;

    triplet_counter.arr[0](a0);
    triplet_counter.arr[1](a1);
    triplet_counter.arr[2](a2);
    triplet_counter.arr[3](a3);
    triplet_counter.arr[4](a4);
    triplet_counter.arr[5](a5);
    triplet_counter.result(res);

    a0 = 1;
    a1 = 3;
    a2 = 5;
    a3 = 10;
    a4 = 14;
    a5 = 15;

    sc_start();

    std::cout << "Number of unique triplets: " << res.read() << std::endl;

    return 0;
}
