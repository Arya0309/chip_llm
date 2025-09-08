int binaryToDecimal(int n)
{
    if (n < 0) return 0;          // Guard against negative input

    int dec_value = 0;
    // Process up to 10 binary digits (bits)
    for (int i = 0; i < 10; ++i)
    {
        int last_digit = n % 10;   // Extract the least‑significant decimal digit (0 or 1)
        n /= 10;                   // Remove that digit
        dec_value += last_digit << i; // Add the bit value shifted to its position
    }
    return dec_value;
}