#include "config.h"

using namespace std;

config::config(std::string fileName) {
	this->fileName = fileName;

	res.profilesCount = 0;
	res.numProfile = 0;
	res.debugMode = false;
	for (int i = 0; i < MAXCOMPUTERS; i++) {
		res.profiles[i].fdd = "";
		res.profiles[i].fddPresent = false;
		res.profiles[i].hdd = "";
		res.profiles[i].hddPresent = false;
		res.profiles[i].name = "";
		res.profiles[i].profileName = "";
		res.profiles[i].ramdisk = false;
		res.profiles[i].ROMPath = "";
	}
	readConfig();
}

config::~config() {


}

bool config::getBoolProperty(string p) {
	if (p == "yes") return true;
	return false;
}

void config::readConfig() {

	string str;
	string comm;
	string prof;

	ifstream file(fileName);
	if (file)
	{
		while (!file.eof())
		{
			file >> str;
			if (str[0] == '[') {
				res.profilesCount++;
				uint8_t i = 1;
				while (str[i] != ']') {
					res.profiles[res.profilesCount - 1].profileName += str[i];
					i++;
				}
			}
			else {
				size_t idx = str.find("=");
				comm = str.substr(0, idx);
				if (comm == "profile")
				{
					prof = str.substr(idx + 1, str.length());
				}
				if (comm == "debugmode")
				{
					res.debugMode = getBoolProperty(str.substr(idx + 1, str.length()));
				}
				if (comm == "name")
				{
					if (res.profilesCount > 0) res.profiles[res.profilesCount - 1].name = str.substr(idx + 1, str.length());
				}
				if (comm == "rom")
				{
					if (res.profilesCount > 0) res.profiles[res.profilesCount - 1].ROMPath = str.substr(idx + 1, str.length());
				}
				if (comm == "ramdisk")
				{
					if (res.profilesCount > 0) res.profiles[res.profilesCount - 1].ramdisk = getBoolProperty(str.substr(idx + 1, str.length()));
				}
				if (comm == "fddcontroller")
				{
					if (res.profilesCount > 0) res.profiles[res.profilesCount - 1].fddPresent = getBoolProperty(str.substr(idx + 1, str.length()));
				}
				if (comm == "fdd")
				{
					if (res.profilesCount > 0) res.profiles[res.profilesCount - 1].fdd = str.substr(idx + 1, str.length());
				}
				if (comm == "hddcontroller")
				{
					if (res.profilesCount > 0) res.profiles[res.profilesCount - 1].hddPresent = getBoolProperty(str.substr(idx + 1, str.length()));
				}
				if (comm == "hdd")
				{
					if (res.profilesCount > 0) res.profiles[res.profilesCount - 1].hdd = str.substr(idx + 1, str.length());
				}
			}
		}
		file.close();

		for (int i = 0; i < res.profilesCount; i++) {
			if (res.profiles[i].profileName == prof) {
				res.numProfile = i;
				break;
			}
		}
	}
	else {
		printf("Error opeining config file: %s\n", fileName.c_str());
		exit(1);
	}
}

CONFIG config::getConfig() {
	return res;
}