#include <cstddef>

// Simple Taylor series approximation of exp(x)
// This is synthesizable and avoids the standard library function.
inline double exp_approx(double x) {
    // Use 10 terms of the series: sum_{k=0}^{9} x^k / k!
    double result = 1.0;      // k = 0 term
    double term   = 1.0;      // current term value
    for (int k = 1; k < 10; ++k) {
        term *= x / static_cast<double>(k); // compute x^k / k!
        result += term;
    }
    return result;
}

template <int N>
void softmax(const double* input, double* output) {
    // Find maximum value for numerical stability
    double max_val = input[0];
    for (int i = 1; i < N; ++i) {
        if (input[i] > max_val) {
            max_val = input[i];
        }
    }

    // Compute exponentials and sum
    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        output[i] = exp_approx(input[i] - max_val);
        sum += output[i];
    }

    // Normalize to obtain softmax probabilities
    for (int i = 0; i < N; ++i) {
        output[i] /= sum;
    }
}