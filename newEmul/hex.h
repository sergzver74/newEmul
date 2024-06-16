#ifndef __HEX_H__
#define __HEX_H__
#include <stdint.h>
#include <string>

uint16_t hexToDec(std::string hex);
std::string decToHexByte(uint8_t b);
std::string decToHexWord(uint16_t b);
std::string decToHex5(uint32_t b);
std::string decToHexDWord(uint32_t b);
std::string getRow(std::string s, int n);

#endif // !__HEX_H__
