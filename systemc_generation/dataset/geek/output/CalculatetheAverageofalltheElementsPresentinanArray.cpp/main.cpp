
#include <systemc.h>

SC_MODULE(AverageCalculator) {
    SC_CTOR(AverageCalculator) {
        SC_THREAD(calculate_average);
    }

    void calculate_average() {
        int arr[] = { 10, 2, 3, 4, 5, 6, 7, 8, 9 };
        int n = sizeof(arr) / sizeof(arr[0]);
        double avg = average(arr, n);
        cout << "Average: " << avg << endl;
        sc_stop();
    }

    double average(int a[], int n) {
        int sum = 0;
        for (int i = 0; i < n; i++)
            sum += a[i];
        return (double)sum / n;
    }
};

int sc_main(int argc, char* argv[]) {
    AverageCalculator calc("calc");
    sc_start();
    return 0;
}
