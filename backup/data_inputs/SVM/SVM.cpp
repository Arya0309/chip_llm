#include <iostream>
using namespace std;

void trainSVM(double* X, int* y,
              int n,
              int d,
              double C,
              double lr,
              int epochs,
              double* w,
              double& b) {
    for (int j = 0; j < d; j++) w[j] = 0.0;
    b = 0.0;

    for (int it = 0; it < epochs; it++) {
        for (int i = 0; i < n; i++) {
            double dot = 0.0;
            for (int j = 0; j < d; j++)
                dot += w[j] * X[i * d + j];
            double margin = y[i] * (dot + b);

            if (margin < 1) {
                for (int j = 0; j < d; j++) {
                    w[j] += lr * (C * y[i] * X[i * d + j] - w[j]);
                }
                b += lr * C * y[i];
            } else {
                for (int j = 0; j < d; j++) {
                    w[j] += lr * (-w[j]);
                }
            }
        }
    }
}

int main() {
    const int n = 4, d = 2;
    double X[n * d] = {
        3.0, 3.0,
        4.0, 3.0,
        1.0, 1.0,
        0.0, 2.0
    };
    int y[n] = { 1, 1, -1, -1 };

    double w[d];
    double b;
    double C = 1.0;
    double lr = 0.001;
    int epochs = 1000;

    trainSVM(X, y, n, d, C, lr, epochs, w, b);

    cout << "Weights: ";
    for (int j = 0; j < d; j++)
        cout << w[j] << " ";
    cout << "\nBias: " << b << endl;

    return 0;
}
