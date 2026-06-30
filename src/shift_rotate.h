// shift_rotate.h
// ROL, ROR, SHL, SHR operations for the assembly interpreter
// Written by: Member 3
//
// All operations convert the signed decimal register value into an
// 8-bit binary representation, manipulate the bits, then convert back
// to decimal before storing into the register.

#ifndef SHIFT_ROTATE_H
#define SHIFT_ROTATE_H

#include <iostream>
using namespace std;

// Converts a signed char (-128..127) into an array of 8 bits.
// bits[0] = MSB (bit 7), bits[7] = LSB (bit 0)
void toBinary(signed char value, int bits[8]) {
    unsigned char uval = (unsigned char)value; // reinterpret as raw bit pattern
    for (int i = 0; i < 8; i++) {
        bits[7 - i] = (uval >> i) & 1;
    }
}

// Converts an 8-bit array (bits[0]=MSB ... bits[7]=LSB) back into a signed char
signed char toDecimal(int bits[8]) {
    unsigned char result = 0;
    for (int i = 0; i < 8; i++) {
        result = (unsigned char)((result << 1) | bits[i]);
    }
    return (signed char)result; // reinterpret back as signed value
}

// Rotate Left: bits that fall off the left wrap around to the right.
// Example (count=1): 0 0 1 0 1 1 1 1 -> 0 1 0 1 1 1 1 0
signed char rotateLeft(signed char value, int count) {
    int bits[8];
    toBinary(value, bits);
    count = count % 8; // rotating by 8 is the same as rotating by 0

    int result[8];
    for (int i = 0; i < 8; i++) {
        result[i] = bits[(i + count) % 8];
    }
    return toDecimal(result);
}

// Rotate Right: bits that fall off the right wrap around to the left.
// Example (count=1): 0 0 1 0 1 1 1 1 -> 1 0 0 1 0 1 1 1
signed char rotateRight(signed char value, int count) {
    int bits[8];
    toBinary(value, bits);
    count = count % 8;

    int result[8];
    for (int i = 0; i < 8; i++) {
        result[i] = bits[(i - count + 8) % 8];
    }
    return toDecimal(result);
}

// Shift Left: bits shift left, rightmost positions filled with 0.
// Shifting 8 or more times always results in 0.
// Example (count=1): 1 0 1 1 0 0 1 1 -> 0 1 1 0 0 1 1 0
signed char shiftLeft(signed char value, int count) {
    if (count >= 8) {
        return 0;
    }
    int bits[8];
    toBinary(value, bits);

    int result[8];
    for (int i = 0; i < 8; i++) {
        if (i + count < 8) {
            result[i] = bits[i + count];
        } else {
            result[i] = 0; // filled with zero
        }
    }
    return toDecimal(result);
}

// Shift Right: bits shift right, leftmost positions filled with 0.
// Shifting 8 or more times always results in 0.
// Example (count=1): 1 0 1 1 0 0 1 1 -> 0 1 0 1 1 0 0 1
signed char shiftRight(signed char value, int count) {
    if (count >= 8) {
        return 0;
    }
    int bits[8];
    toBinary(value, bits);

    int result[8];
    for (int i = 0; i < 8; i++) {
        if (i - count >= 0) {
            result[i] = bits[i - count];
        } else {
            result[i] = 0; // filled with zero
        }
    }
    return toDecimal(result);
}

#endif