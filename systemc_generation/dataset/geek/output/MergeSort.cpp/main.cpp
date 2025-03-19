
#include <systemc.h>
#include <vector>

SC_MODULE(MergeSortModule) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> start;
    sc_out<bool> done;
    sc_fifo_in<int> data_in;
    sc_fifo_out<int> data_out;

    SC_CTOR(MergeSortModule) {
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void sort_process() {
        if (reset.read()) {
            done.write(false);
        } else if (start.read()) {
            std::vector<int> vec;
            int val;
            while (data_in.num_available() > 0) {
                data_in.read(val);
                vec.push_back(val);
            }
            mergeSort(vec, 0, vec.size() - 1);
            for (auto i : vec) {
                data_out.write(i);
            }
            done.write(true);
        }
    }

    void merge(std::vector<int>& vec, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        std::vector<int> leftVec(n1), rightVec(n2);
        for (int i = 0; i < n1; i++) leftVec[i] = vec[left + i];
        for (int j = 0; j < n2; j++) rightVec[j] = vec[mid + 1 + j];
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (leftVec[i] <= rightVec[j]) {
                vec[k] = leftVec[i];
                i++;
            } else {
                vec[k] = rightVec[j];
                j++;
            }
            k++;
        }
        while (i < n1) {
            vec[k] = leftVec[i];
            i++;
            k++;
        }
        while (j < n2) {
            vec[k] = rightVec[j];
            j++;
            k++;
        }
    }

    void mergeSort(std::vector<int>& vec, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(vec, left, mid);
            mergeSort(vec, mid + 1, right);
            merge(vec, left, mid, right);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    MergeSortModule merge_sort("merge_sort");
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset("reset");
    sc_signal<bool> start("start");
    sc_signal<bool> done("done");
    sc_fifo<int> data_in("data_in", 10);
    sc_fifo<int> data_out("data_out", 10);

    merge_sort.clk(clk);
    merge_sort.reset(reset);
    merge_sort.start(start);
    merge_sort.done(done);
    merge_sort.data_in(data_in);
    merge_sort.data_out(data_out);

    reset.write(true);
    sc_start(10, SC_NS);
    reset.write(false);
    start.write(true);

    std::vector<int> input = {12, 11, 13, 5, 6, 7};
    for (int i : input) {
        data_in.write(i);
    }

    while (!done.read()) {
        sc_start(10, SC_NS);
    }

    std::cout << "Sorted output: ";
    while (data_out.num_available() > 0) {
        std::cout << data_out.read() << " ";
    }
    std::cout << std::endl;

    sc_stop();
    return 0;
}
