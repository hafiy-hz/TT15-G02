// test_shift_rotate.cpp
// Verifies rotateLeft, rotateRight, shiftLeft, shiftRight against the
// EXACT bit patterns shown in the assignment PDF.

#include "../src/shift_rotate.h"
#include <iostream>
using namespace std;

void printBits(signed char value) {
    int bits[8];
    toBinary(value, bits);
    for (int i = 0; i < 8; i++) cout << bits[i];
}

int main() {
    cout << "=== ROL test (PDF example: ROL R1,1) ===" << endl;
    // PDF shows: 0 0 1 0 1 1 1 1  ->  0 1 0 1 1 1 1 0
    int startBits[8] = {0,0,1,0,1,1,1,1};
    signed char startVal = toDecimal(startBits);
    cout << "Start binary: "; printBits(startVal); cout << " (decimal: " << (int)startVal << ")" << endl;

    signed char rolResult = rotateLeft(startVal, 1);
    cout << "After ROL 1: "; printBits(rolResult); cout << " (decimal: " << (int)rolResult << ")" << endl;
    cout << "Expected:    01011110 (verified by manual left-rotate calculation)" << endl;

    cout << "\n=== ROR test (PDF example: ROR R1,1) ===" << endl;
    // Using same start value, PDF shows similar wraparound logic for ROR
    signed char rorResult = rotateRight(startVal, 1);
    cout << "After ROR 1: "; printBits(rorResult); cout << " (decimal: " << (int)rorResult << ")" << endl;
    cout << "Expected:    10010111" << endl;

    cout << "\n=== SHL test (PDF example: SHL R1,1) ===" << endl;
    // PDF shows: 1 0 1 1 0 0 1 1  ->  0 1 1 0 0 1 1 0
    int shlStartBits[8] = {1,0,1,1,0,0,1,1};
    signed char shlStart = toDecimal(shlStartBits);
    cout << "Start binary: "; printBits(shlStart); cout << " (decimal: " << (int)shlStart << ")" << endl;

    signed char shlResult = shiftLeft(shlStart, 1);
    cout << "After SHL 1: "; printBits(shlResult); cout << " (decimal: " << (int)shlResult << ")" << endl;
    cout << "Expected:    01100110" << endl;

    cout << "\n=== SHR test (PDF example: SHR R1,1) ===" << endl;
    // PDF shows: 1 0 1 1 0 0 1 1  ->  0 1 0 1 1 0 0 1
    signed char shrResult = shiftRight(shlStart, 1);
    cout << "After SHR 1: "; printBits(shrResult); cout << " (decimal: " << (int)shrResult << ")" << endl;
    cout << "Expected:    01011001" << endl;

    cout << "\n=== Edge case: SHL by 7+ positions should result in 0 ===" << endl;
    signed char shlEdge = shiftLeft(shlStart, 7);
    cout << "After SHL 7: " << (int)shlEdge << " (expected 0)" << endl;
    signed char shlEdge8 = shiftLeft(shlStart, 8);
    cout << "After SHL 8: " << (int)shlEdge8 << " (expected 0)" << endl;

    cout << "\n=== Edge case: negative numbers ===" << endl;
    signed char negVal = -1; // 11111111 in binary
    cout << "Start: -1 binary: "; printBits(negVal); cout << endl;
    signed char negShr = shiftRight(negVal, 1);
    cout << "After SHR 1: "; printBits(negShr); cout << " (decimal: " << (int)negShr << ", expected 127)" << endl;

    cout << "\nAll comparisons above should match 'Expected' values." << endl;
    return 0;
}