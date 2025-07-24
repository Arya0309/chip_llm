#include "Testbench.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/* === Fixed Format === */
Testbench::Testbench(sc_module_name n)
    : sc_module(n) {
    SC_THREAD(do_feed);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(do_fetch);
    sensitive << i_clk.pos();
    dont_initialize();
}
/* === Fixed Format End === */

void Testbench::do_feed() {
    std::vector<std::pair<std::array<unsigned char, 16>, std::array<unsigned char, 16>>> testcases;
    std::ifstream fin("testcases.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt" << std::endl;
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::array<unsigned char, 16> plaintext, key;
        for (int i = 0; i < 16; ++i) {
            if (!(iss >> std::hex >> plaintext[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
                continue;
            }
        }
        for (int i = 0; i < 16; ++i) {
            if (!(iss >> std::hex >> key[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
                continue;
            }
        }
        testcases.push_back({plaintext, key});
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto& [plaintext, key] : testcases) {
        for (int i = 0; i < 16; ++i) {
            o_plaintext.write(plaintext[i]);
            o_key.write(key[i]);
        }
        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<std::array<unsigned char, 16>> expected_ciphertexts;
    std::ifstream fin("expected_ciphertexts.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open expected_ciphertexts.txt" << std::endl;
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::array<unsigned char, 16> ciphertext;
        for (int i = 0; i < 16; ++i) {
            if (!(iss >> std::hex >> ciphertext[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << std::endl;
                continue;
            }
        }
        expected_ciphertexts.push_back(ciphertext);
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < expected_ciphertexts.size(); ++idx) {
        std::array<unsigned char, 16> ciphertext;
        for (int i = 0; i < 16; ++i) {
            ciphertext[i] = i_ciphertext.read();
        }

        bool passed = (ciphertext == expected_ciphertexts[idx]);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed." << std::endl;
            std::cout << "Plaintext: " << std::hex;
            for (int i = 0; i < 16; ++i) {
                std::cout << static_cast<int>(testcases[idx].first[i]) << " ";
            }
            std::cout << std::endl;
            std::cout << "Key: " << std::hex;
            for (int i = 0; i < 16; ++i) {
                std::cout << static_cast<int>(testcases[idx].second[i]) << " ";
            }
            std::cout << std::endl;
            std::cout << "Ciphertext: " << std::hex;
            for (int i = 0; i < 16; ++i) {
                std::cout << static_cast<int>(ciphertext[i]) << " ";
            }
            std::cout << std::endl;
        } else {
            std::cerr << "Test case " << idx + 1 << " failed." << std::endl;
            std::cerr << "Plaintext: " << std::hex;
            for (int i = 0; i < 16; ++i) {
                std::cerr << static_cast<int>(testcases[idx].first[i]) << " ";
            }
            std::cerr << std::endl;
            std::cerr << "Key: " << std::hex;
            for (int i = 0; i < 16; ++i) {
                std::cerr << static_cast<int>(testcases[idx].second[i]) << " ";
            }
            std::cerr << std::endl;
            std::cerr << "Ciphertext: " << std::hex;
            for (int i = 0; i < 16; ++i) {
                std::cerr << static_cast<int>(ciphertext[i]) << " ";
            }
            std::cerr << std::endl;
            std::cerr << "Expected Ciphertext: " << std::hex;
            for (int i = 0; i < 16; ++i) {
                std::cerr << static_cast<int>(expected_ciphertexts[idx][i]) << " ";
            }
            std::cerr << std::endl;
            all_passed = false;
        }
    }

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
