
#include <systemc.h>

SC_MODULE(ArmstrongFinder) {
    SC_CTOR(ArmstrongFinder) {
        SC_THREAD(find_armstrong_numbers);
    }

    void find_armstrong_numbers() {
        cout << "Armstrong numbers between 1 to 1000 : ";
        for (int num = 1; num <= 1000; ++num) {
            if (isArmstrong(num)) {
                cout << num << " ";
            }
        }
        sc_stop();
    }

    int order(int num) {
        int count = 0;
        while (num > 0) {
            num /= 10;
            count++;
        }
        return count;
    }

    bool isArmstrong(int num) {
        int order_n = order(num);
        int num_temp = num, sum = 0;

        while (num_temp > 0) {
            int curr = num_temp % 10;
            sum += pow(curr, order_n);
            num_temp /= 10;
        }
        return (sum == num);
    }
};

int sc_main(int argc, char* argv[]) {
    ArmstrongFinder armstrong_finder("armstrong_finder");
    sc_start();
    return 0;
}
