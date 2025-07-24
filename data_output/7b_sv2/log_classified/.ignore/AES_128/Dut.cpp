
#include "Dut.h"

#include <iostream>
using namespace std;

const unsigned char sbox[256] = { /* S-box values */ };
const unsigned char Rcon[11] = { /* Rcon values */ };

unsigned char xtime(unsigned char x) {
    return (unsigned char)((x << 1) ^ ((x & 0x80) ? 0x1B : 0x00));
}

void SubBytes(unsigned char state[4][4]) {
    for(int r=0;r<4;r++)
        for(int c=0;c<4;c++)
            state[r][c] = sbox[state[r][c]];
}

void ShiftRows(unsigned char state[4][4]) {
    unsigned char tmp;
    // row 1
    tmp = state[1][0];
    for(int c=0;c<3;c++) state[1][c] = state[1][c+1];
    state[1][3] = tmp;
    // row 2
    swap(state[2][0], state[2][2]);
    swap(state[2][1], state[2][3]);
    // row 3
    tmp = state[3][3];
    for(int c=3;c>0;c--) state[3][c] = state[3][c-1];
    state[3][0] = tmp;
}

void MixColumns(unsigned char state[4][4]) {
    for(int c=0;c<4;c++) {
        unsigned char a0 = state[0][c], a1 = state[1][c], a2 = state[2][c], a3 = state[3][c];
        unsigned char m0 = xtime(a0) ^ (xtime(a1) ^ a1) ^ a2 ^ a3;
        unsigned char m1 = a0 ^ xtime(a1) ^ (xtime(a2) ^ a2) ^ a3;
        unsigned char m2 = a0 ^ a1 ^ xtime(a2) ^ (xtime(a3) ^ a3);
        unsigned char m3 = (xtime(a0) ^ a0) ^ a1 ^ a2 ^ xtime(a3);
        state[0][c] = m0;
        state[1][c] = m1;
        state[2][c] = m2;
        state[3][c] = m3;
    }
}

void AddRoundKey(unsigned char state[4][4], const unsigned char roundKey[16]) {
    for(int r=0;r<4;r++)
        for(int c=0;c<4;c++)
            state[r][c] ^= roundKey[c*4 + r];
}

void KeyExpansion(const unsigned char key[16], unsigned char roundKeys[176]) {
    for(int i=0;i<16;i++) roundKeys[i] = key[i];
    int bytesGenerated = 16;
    int rconIter = 1;
    unsigned char temp[4];
    while(bytesGenerated < 176) {
        for(int i=0;i<4;i++) temp[i] = roundKeys[bytesGenerated - 4 + i];
        if(bytesGenerated % 16 == 0) {
            // rotate
            unsigned char t = temp[0];
            temp[0]=temp[1]; temp[1]=temp[2]; temp[2]=temp[3]; temp[3]=t;
            // subbytes
            for(int i=0;i<4;i++) temp[i] = sbox[temp[i]];
            // Rcon
            temp[0] ^= Rcon[rconIter];
            rconIter++;
        }
        for(int i=0;i<4;i++) {
            roundKeys[bytesGenerated] = roundKeys[bytesGenerated - 16] ^ temp[i];
            bytesGenerated++;
        }
    }
}

void AES128Encrypt(const unsigned char in[16], const unsigned char key[16], unsigned char out[16]) {
    unsigned char state[4][4];
    unsigned char roundKeys[176];
    KeyExpansion(key, roundKeys);
    // load state
    for(int r=0;r<4;r++)
        for(int c=0;c<4;c++)
            state[r][c] = in[c*4 + r];
    // initial round
    AddRoundKey(state, roundKeys);
    // 9 main rounds
    for(int round=1; round<=9; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, roundKeys + round*16);
    }
    // final round
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, roundKeys + 160);
    // output
    for(int r=0;r<4;r++)
        for(int c=0;c<4;c++)
            out[c*4 + r] = state[r][c];
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
        unsigned char plaintext[16];
        unsigned char key[16];
        unsigned char ciphertext[16];
        // read plaintext and key element-by-element
        for (int i = 0; i < 16; ++i) {
            plaintext[i] = i_plaintext.read();
        }
        for (int i = 0; i < 16; ++i) {
            key[i] = i_key.read();
        }
        /* === Variable Section End === */

        /* === Main function Section === */
        AES128Encrypt(plaintext, key, ciphertext);
        /* === Main function Section End === */

        /* === Variable Section === */
        // write ciphertext back out
        for (int i = 0; i < 16; ++i) {
            o_ciphertext.write(ciphertext[i]);
        }
        /* === Variable Section End === */
    }
}
