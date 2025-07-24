
#include "Dut.h"

#include <cmath>

const double PI = 3.14159265358979323846;

// Approximate sine function
double sin_approx(double x) {
    while (x > PI)  x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    double x3 = x * x * x;
    double x5 = x3 * x * x;
    double x7 = x5 * x * x;
    return x - x3 / 6.0 + x5 / 120.0 - x7 / 5040.0;
}

// Approximate cosine function
double cos_approx(double x) {
    while (x > PI)  x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    double x2 = x * x;
    double x4 = x2 * x2;
    double x6 = x4 * x2;
    return 1 - x2 / 2.0 + x4 / 24.0 - x6 / 720.0;
}

// Complex number structure
struct Complex {
    double real;
    double imag;
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}
    Complex operator+(const Complex &other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    Complex operator-(const Complex &other) const {
        return Complex(real - other.real, imag - other.imag);
    }
    Complex operator*(const Complex &other) const {
        return Complex(real * other.real - imag * other.imag, real * other.imag + imag * other.real);
    }
};

// FFT function
void fft(Complex *a, int n) {
    if (n <= 1) return;
    Complex *even = new Complex[n/2];
    Complex *odd  = new Complex[n/2];
    for (int i = 0; i < n/2; ++i) {
        even[i] = a[2*i];
        odd[i]  = a[2*i + 1];
    }
    fft(even, n/2);
    fft(odd,  n/2);
    for (int k = 0; k < n/2; ++k) {
        double angle = -2 * PI * k / n;
        Complex w(cos_approx(angle), sin_approx(angle));
        Complex t = w * odd[k];
        a[k]         = even[k] + t;
        a[k + n/2]   = even[k] - t;
    }
    delete[] even;
    delete[] odd;
}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {
    wait();
    while (true) {
        /* === Variable Section === */
        Complex a[8];
        // Read input array element-by-element
        for (int i = 0; i < 8; ++i) {
            a[i].real = i_a_real.read();
            a[i].imag = i_a_imag.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        fft(a, 8);
        /* === Main function Section End === */

        /* === Variable Section === */
        // Write result array back out
        for (int i = 0; i < 8; ++i) {
            o_result_real.write(a[i].real);
            o_result_imag.write(a[i].imag);
        }
        /* === Variable Section End === */
    }
}
