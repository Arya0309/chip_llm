/*────────────────────────  HexToDecimal.sv  ────────────────────────*/
#include <systemc.h>
#include <cassert>
#include <string>

/* ──────────────── 被測模組：十六進位 → 十進位 ──────────────── */
SC_MODULE(HexToDecimal) {
    sc_in<char> hex_in[8];   // 8-byte null-terminated hex 字串
    sc_out<int> dec_out;     // 轉換後的十進位結果

    SC_CTOR(HexToDecimal) {
        SC_METHOD(convert);
        dont_initialize();
        for (int i = 0; i < 8; ++i)    // 每個字元改變就重新計算
            sensitive << hex_in[i];
    }

    void convert() {
        /* 1. 組成輸入字串 */
        std::string hex;
        for (int i = 0; i < 8; ++i) {
            char c = hex_in[i].read();
            if (c == '\0') break;
            hex += c;
        }

        /* 2. 套用與原 C++ 相同的轉換演算法 */
        int base     = 1;
        int dec_val  = 0;
        for (int i = static_cast<int>(hex.size()) - 1; i >= 0; --i) {
            char ch = hex[i];
            if (ch >= '0' && ch <= '9')
                dec_val += (ch - '0') * base;
            else if (ch >= 'A' && ch <= 'F')
                dec_val += (ch - 'A' + 10) * base;
            else if (ch >= 'a' && ch <= 'f')
                dec_val += (ch - 'a' + 10) * base;
            base *= 16;
        }
        dec_out.write(dec_val);
    }
};

/* ──────────────────────── Testbench ──────────────────────── */
SC_MODULE(Testbench) {
    sc_signal<char> sig_hex[8];
    sc_signal<int>  sig_dec;

    HexToDecimal dut{"HexToDecimal"};

    SC_CTOR(Testbench) {
        for (int i = 0; i < 8; ++i)
            dut.hex_in[i](sig_hex[i]);
        dut.dec_out(sig_dec);

        SC_THREAD(run_tests);
    }

    /* 將 C-style 字串寫入輸入腳位（自動補 \0） */
    void write_hex(const std::string& s) {
        int i = 0;
        for (; i < static_cast<int>(s.size()) && i < 8; ++i)
            sig_hex[i].write(s[i]);
        if (i < 8) sig_hex[i++].write('\0');
        for (; i < 8; ++i) sig_hex[i].write('\0');
    }

    /* 測試流程 */
    void run_tests() {
        // ▸ 測試 1：1A → 26
        write_hex("1A");
        wait(1, SC_NS);                 // 觸發更新
        assert(sig_dec.read() == 26);
        std::cout << "Hex 1A -> " << sig_dec.read() << '\n';

        // ▸ 測試 2：FF → 255
        write_hex("FF");
        wait(1, SC_NS);
        assert(sig_dec.read() == 255);
        std::cout << "Hex FF -> " << sig_dec.read() << '\n';

        // ▸ 測試 3：1234 → 4660
        write_hex("1234");
        wait(1, SC_NS);
        assert(sig_dec.read() == 0x1234);
        std::cout << "Hex 1234 -> " << sig_dec.read() << '\n';

        std::cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

/* ──────────────────────── sc_main ──────────────────────── */
int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
