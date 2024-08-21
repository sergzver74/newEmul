#include "fs.h"

#pragma comment(lib, "shlwapi.lib")

string* getDisks() {

	string* names = new string[32];
	for (int i = 0; i < 32; i++) names[i] = "";

	uint32_t disks = GetLogicalDrives();
	printf("disks: %d\n", disks);

	int i = 0;

	char buf[255];
	int sz = GetLogicalDriveStringsA(sizeof(buf), buf);
	if (sz > 0)
	{
		char* p1 = buf;
		char* p2;

		char drive_label[256];
		char drive_label1[256];
		char drive_fat[30];
		DWORD drive_sn;
		DWORD drive_name_size = sizeof(drive_label);

		while (*p1 != '\0' && (p2 = strchr(p1, '\0')) != NULL)
		{

			bool Flag = GetVolumeInformationA(p1,
				drive_label,
				sizeof(drive_label),
				&drive_sn,
				&drive_name_size,
				NULL,
				drive_fat,
				sizeof(drive_fat)
			);
			if (Flag)
			{
				//if (drive_label[0]!=0) CharToOemBuffA(drive_label, drive_label1, 256); else CharToOemBuffA("Локальный диск", drive_label1, 256);

				//names[i] = string(drive_label1) + " (" + string(p1,2) + ")";
				if (drive_label[0] != 0) names[i] = string(drive_label) + " (" + string(p1, 2) + ")"; else names[i] = string("Локальный диск") + " (" + string(p1, 2) + ")";
				printf("%s\n", names[i].c_str());
				i++;
			}

			p1 = p2 + 1;
		}
	}
	return names;
}

string GetThisPath()
{
	char dest[MAX_PATH];

	DWORD length = GetModuleFileNameA(NULL, dest, MAX_PATH);
	PathRemoveFileSpecA(dest);
	return string(dest);
}

void sort(WIN32_FIND_DATAA* fd, uint16_t n) {
	if (n > 1)
	{
		for (int i = 0; i < n - 1; i++) {
			int min = i;
			for (int j = i + 1; j < n; j++) {
				if (strcmp(fd[j].cFileName, fd[min].cFileName) < 0)
					min = j;
			}
			if (min != i)
			{
				WIN32_FIND_DATAA f = fd[i];
				fd[i] = fd[min];
				fd[min] = f;
			}
		}
	}
}

WIN32_FIND_DATAA* getFilesList(string path, uint32_t* n, string mask) {
	WIN32_FIND_DATAA f;
	WIN32_FIND_DATAA* fd;
	WIN32_FIND_DATAA* fd1;
	fd = new WIN32_FIND_DATAA[1024];
	fd1 = new WIN32_FIND_DATAA[1024];
	int i = 0;
	int k = 0;
	string nPath = path + "\\*.*";
	HANDLE hFind = FindFirstFileA(nPath.c_str(), &f);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				printf("  %s   <DIR>\n", f.cFileName);
				fd[i] = f;
				i++;
			}
			else {
				printf("  %s   %d bytes\n", f.cFileName, f.nFileSizeLow);
				string nm = string(f.cFileName);
				if (mask == "*.*") {
					fd1[k] = f;
					k++;
				}
				else {
					if (nm[nm.length() - 4] == mask[mask.length() - 4] && nm[nm.length() - 3] == mask[mask.length() - 3] && nm[nm.length() - 2] == mask[mask.length() - 2] && nm[nm.length() - 1] == mask[mask.length() - 1]) {
						fd1[k] = f;
						k++;
					}
				}
			}
		} while (FindNextFileA(hFind, &f));
		FindClose(hFind);
	}

	sort(fd, i);
	sort(fd1, k);

	WIN32_FIND_DATAA* fdNew;
	fdNew = new WIN32_FIND_DATAA[i + k];
	for (int l = 0; l < i; l++) fdNew[l] = fd[l];
	for (int l = 0; l < k; l++) fdNew[i + l] = fd1[l];
	delete[] fd1;
	delete[] fd;

	*n = i + k;
	return fdNew;
}
