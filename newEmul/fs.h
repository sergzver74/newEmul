#ifndef __FS_H__
#define __FS_H__

#include "string"
#include <shlwapi.h>

using namespace std;

string* getDisks();
string GetThisPath();
void sort(WIN32_FIND_DATAA* fd, uint16_t n);
WIN32_FIND_DATAA* getFilesList(string path, uint32_t* n, string mask);

#endif // !__FS_H__

