#include "hex.h"
#include <string.h>

uint16_t hexToDec(std::string hex) {
	uint16_t res = 0;
	for (int i = 0; i < hex.length(); i++) {
		uint16_t code = 0;
		if (hex[i] >= '0' && hex[i] <= '9') code = hex[i] - 0x30;
		if (hex[i] >= 'a' && hex[i] <= 'f') code = hex[i] - 'a' + 10;
		if (hex[i] >= 'A' && hex[i] <= 'F') code = hex[i] - 'A' + 10;
		res = res * 16 + code;
	}
	return res;
}

std::string decToHexByte(uint8_t b) {
	uint8_t buf[16];
	sprintf_s((char*)buf, 16, "%02X\0", b);
	std::string res((char*)buf);
	return res;
}

std::string decToHexWord(uint16_t b) {
	uint8_t buf[16];
	sprintf_s((char*)buf, 16, "%04X\0", b);
	std::string res((char*)buf);
	return res;
}

std::string decToHex5(uint32_t b) {
	uint8_t buf[16];
	sprintf_s((char*)buf, 16, "%05X\0", b);
	std::string res((char*)buf);
	return res;
}

std::string decToHexDWord(uint32_t b) {
	uint8_t buf[16];
	sprintf_s((char*)buf, 16, "%08X\0", b);
	std::string res((char*)buf);
	return res;
}

std::string getRow(std::string s, int n) {
	std::string res = "";

	int k = 0;
	int i = 0;
	while (s[i]!='\0')
	{
		if (s[i] != 0x0A) {
			res += s[i];
		}
		else {
			if (k == n) break;
			res = "";
			k++;
		}
		i++;
	}


	return res;
}