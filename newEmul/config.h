#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string>

#define MAXCOMPUTERS	8

typedef struct {
	std::string profileName;
	std::string ROMPath;
	std::string name;
	bool ramdisk;
	bool fddPresent;
	bool hddPresent;
	std::string fdd;
	std::string hdd;
}PROFILE;

typedef struct {
	bool debugMode;
	uint8_t numProfile;
	uint8_t profilesCount;
	PROFILE profiles[MAXCOMPUTERS];
}CONFIG;

class config {
private:
	std::string fileName;
	bool getBoolProperty(std::string p);
	void readConfig();
	CONFIG res;
public:
	config(std::string fileName);
	~config();
	CONFIG getConfig();

};

#endif // !__CONFIG_H__

