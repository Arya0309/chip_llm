#include <iostream>
#include <fstream>
using namespace std;

double* conv2D(
    const double* input, int in_h, int in_w,
    const double* kernel, int ker_h, int ker_w)
{
    int pad_h = ker_h / 2;
    int pad_w = ker_w / 2;
    int padded_h = in_h + 2 * pad_h;
    int padded_w = in_w + 2 * pad_w;
    int out_h = in_h;
    int out_w = in_w;

    double* padded = new double[padded_h * padded_w]();
    double* output = new double[out_h * out_w];

    for (int i = 0; i < in_h; ++i) {
        for (int j = 0; j < in_w; ++j) {
            padded[(i + pad_h) * padded_w + (j + pad_w)] = input[i * in_w + j];
        }
    }

    for (int i = 0; i < out_h; ++i) {
        for (int j = 0; j < out_w; ++j) {
            double sum = 0.0;
            for (int m = 0; m < ker_h; ++m) {
                for (int n = 0; n < ker_w; ++n) {
                    sum += padded[(i + m) * padded_w + (j + n)]
                         * kernel[m * ker_w + n];
                }
            }
            output[i * out_w + j] = sum;
        }
    }

    delete[] padded;
    return output;
}

int main() {
    ifstream infile("testcases.txt");
    ofstream outfile("ans.txt");

    const int in_h = 4, in_w = 4;
    const int ker_h = 3, ker_w = 3;
    const int total_out = in_h * in_w;

    double input[in_h * in_w];
    double kernel[ker_h * ker_w];

    int count = 0;
    while (count < 100 && infile >> input[0]) {
        for (int i = 1; i < in_h * in_w; ++i) {
            infile >> input[i];
        }
        for (int i = 0; i < ker_h * ker_w; ++i) {
            infile >> kernel[i];
        }
        double* result = conv2D(input, in_h, in_w, kernel, ker_h, ker_w);
        for (int i = 0; i < total_out; ++i) {
            outfile << result[i] << (i == total_out - 1 ? "" : " ");
        }
        outfile << endl;
        delete[] result;
        count++;
    }
    infile.close();
    outfile.close();
    return 0;
}
