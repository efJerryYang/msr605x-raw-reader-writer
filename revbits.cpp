#include <iostream>
#include <cassert>
unsigned char reverseBits(unsigned char byte) {
    unsigned char reversed = 0;
    for (int i = 0; i < 8; i++) {
        reversed = (reversed << 1) | (byte & 1);
        byte >>= 1;
    }
    return reversed;
}

int main() {
    unsigned char input[8] = {
        0b1101'0010, 
        0b1010'1111, 
        0b0101'1101, 
        0b0111'1100, 

        0b1011'1011, 
        0b0010'0100, 
        0b1101'0101, 
        0b0110'0100, 
    };
    unsigned char output[8] = {
        0b0100'1011,
        0b1111'0101,
        0b1011'1010,
        0b0011'1110,

        0b1101'1101,
        0b0010'0100,
        0b1010'1011,
        0b0010'0110,
    };

    for (int i = 0; i < 8; i++) {
        input[i] = reverseBits(input[i]);
    }

    for (int i = 0; i < 8; i++) {
        assert(input[i] == output[i]);
    }
    std::cout << "All tests passed!" << std::endl;

    return 0;
}