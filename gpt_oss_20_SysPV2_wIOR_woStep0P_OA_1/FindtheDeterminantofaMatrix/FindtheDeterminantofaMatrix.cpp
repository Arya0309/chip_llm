#include <cmath>

template<int N>
void swapRows(double (&a)[N][N], int i, int j) {
    for (int k = 0; k < N; ++k) {
        double tmp = a[i][k];
        a[i][k] = a[j][k];
        a[j][k] = tmp;
    }
}

template<int N>
double determinant(double (&a)[N][N]) {
    double det = 1.0;
    for (int i = 0; i < N; ++i) {
        int pivot = i;
        for (int j = i + 1; j < N; ++j) {
            if (std::abs(a[j][i]) > std::abs(a[pivot][i])) {
                pivot = j;
            }
        }
        if (pivot != i) {
            swapRows(a, i, pivot);
            det *= -1.0;
        }
        if (a[i][i] == 0.0) {
            return 0.0;
        }
        det *= a[i][i];
        for (int j = i + 1; j < N; ++j) {
            double factor = a[j][i] / a[i][i];
            for (int k = i + 1; k < N; ++k) {
                a[j][k] -= factor * a[i][k];
            }
        }
    }
    return det;
}