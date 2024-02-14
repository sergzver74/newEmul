#ifndef __EMUTYPES_H__
#define __EMUTYPES_H__

#include <string>

using namespace std;

typedef struct {
	string name;
	bool data;
	string strData;
} FlagEntry;

typedef struct {
	uint8_t count;
	FlagEntry flags[32];
} Flags;

typedef struct {
	string name;
	uint32_t data;
	string strData;
} RegEntry;

typedef struct {
	uint8_t count;
	RegEntry registers[128];
	Flags flags;
} Registers;

#endif // !__EMUTYPES_H__
