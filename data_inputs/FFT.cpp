#include <iostream>
using namespace std;

// 常數 π
const double PI = 3.14159265358979323846;

// 自訂複數結構
struct Complex {
    double re, im;
    Complex(double r = 0, double i = 0) : re(r), im(i) {}
    Complex operator+(const Complex &o) const { return Complex(re + o.re, im + o.im); }
    Complex operator-(const Complex &o) const { return Complex(re - o.re, im - o.im); }
    Complex operator*(const Complex &o) const { 
        return Complex(re * o.re - im * o.im, re * o.im + im * o.re); 
    }
};

// 泰勒展開近似 sin(x)
double sin_approx(double x) {
    // 把 x 綁定到 [-π, π]
    while (x > PI)  x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    double x3 = x * x * x;
    double x5 = x3 * x * x;
    double x7 = x5 * x * x;
    return x - x3 / 6.0 + x5 / 120.0 - x7 / 5040.0;
}

// 泰勒展開近似 cos(x)
double cos_approx(double x) {
    // 把 x 綁定到 [-π, π]
    while (x > PI)  x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    double x2 = x * x;
    double x4 = x2 * x2;
    double x6 = x4 * x2;
    return 1 - x2 / 2.0 + x4 / 24.0 - x6 / 720.0;
}

// 遞迴版 Cooley–Tukey FFT，輸入長度必須為 2 的冪次
void fft(Complex *a, int n) {
    if (n <= 1) return;
    // 分割為偶數與奇數兩組
    Complex *even = new Complex[n/2];
    Complex *odd  = new Complex[n/2];
    for (int i = 0; i < n/2; ++i) {
        even[i] = a[2*i];
        odd[i]  = a[2*i + 1];
    }
    // 遞迴計算
    fft(even, n/2);
    fft(odd,  n/2);
    // 合併
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

int main() {
    // 範例輸入：長度為 8 的序列
    int n = 8;
    Complex data[8];
    for (int i = 0; i < n; ++i) {
        data[i] = Complex(i, 0);  // 實部為 0,1,2,...7；虛部為 0
    }

    // 執行 FFT
    fft(data, n);

    // 輸出結果
    for (int i = 0; i < n; ++i) {
        cout << "FFT[" << i << "] = "
             << data[i].re << " + " << data[i].im << "i\n";
    }
    return 0;
}
