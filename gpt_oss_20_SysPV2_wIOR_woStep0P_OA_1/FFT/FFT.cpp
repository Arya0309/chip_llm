const double PI = 3.14159265358979323846;

struct Complex {
    double re, im;
    Complex(double r = 0, double i = 0) : re(r), im(i) {}
    Complex operator+(const Complex &o) const { return Complex(re + o.re, im + o.im); }
    Complex operator-(const Complex &o) const { return Complex(re - o.re, im - o.im); }
    Complex operator*(const Complex &o) const {
        return Complex(re * o.re - im * o.im, re * o.im + im * o.re);
    }
};

double sin_approx(double x) {
    // Assumes x is already in [-PI, PI]
    double x3 = x * x * x;
    double x5 = x3 * x * x;
    double x7 = x5 * x * x;
    return x - x3 / 6.0 + x5 / 120.0 - x7 / 5040.0;
}

double cos_approx(double x) {
    // Assumes x is already in [-PI, PI]
    double x2 = x * x;
    double x4 = x2 * x2;
    double x6 = x4 * x2;
    return 1 - x2 / 2.0 + x4 / 24.0 - x6 / 720.0;
}

/* Iterative 8‑point FFT (in‑place) */
void fft(Complex a[8]) {
    /* Bit‑reversal permutation */
    Complex temp[8];
    temp[0] = a[0];
    temp[1] = a[4];
    temp[2] = a[2];
    temp[3] = a[6];
    temp[4] = a[1];
    temp[5] = a[5];
    temp[6] = a[3];
    temp[7] = a[7];
    for (int i = 0; i < 8; ++i) a[i] = temp[i];

    /* Stage 1: length‑2 butterflies */
    for (int i = 0; i < 8; i += 2) {
        Complex w(1.0, 0.0);
        Complex t = w * a[i + 1];
        a[i + 1] = a[i] - t;
        a[i]     = a[i] + t;
    }

    /* Stage 2: length‑4 butterflies */
    for (int i = 0; i < 8; i += 4) {
        for (int j = 0; j < 2; ++j) {
            double angle = -2 * PI * j / 4.0;  // angle ∈ [-PI, 0]
            Complex w(cos_approx(angle), sin_approx(angle));
            Complex t = w * a[i + 2 + j];
            a[i + 2 + j] = a[i + j] - t;
            a[i + j]     = a[i + j] + t;
        }
    }

    /* Stage 3: length‑8 butterflies */
    for (int i = 0; i < 8; i += 8) {
        for (int j = 0; j < 4; ++j) {
            double angle = -2 * PI * j / 8.0;  // angle ∈ [-PI, 0]
            Complex w(cos_approx(angle), sin_approx(angle));
            Complex t = w * a[i + 4 + j];
            a[i + 4 + j] = a[i + j] - t;
            a[i + j]     = a[i + j] + t;
        }
    }
}