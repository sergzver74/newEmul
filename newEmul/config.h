#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string>

typedef struct {
	std::string profileName;
	std::string ROMPath;
	std::string name;
	bool ramdisk;
	std::string fdd;
	std::string hdd;
}PROFILE;

typedef struct {
	bool debugMode;
	uint8_t numProfile;
	uint8_t profilesCount;
	PROFILE profiles[8];
}CONFIG;

class config {
private:
	std::string fileName;
	bool getBoolProperty(std::string p);
public:
	config(std::string fileName);
	~config();
	CONFIG getConfig();

};

#endif // !__CONFIG_H__

