// CProfile.h

/*
 * $Id: Cprofile.h 4 2013-10-16 08:25:28Z STONERIDGE\mima $
 */

#ifndef __CPROFILE_H
#define __CPROFILE_H

#include <windows.h>

#define MAX_FILENAME_LENGTH 128

class C_PrivProfile
{
public:
	 C_PrivProfile(char *sFileName);
	 BOOL Add(char *sSection, char *sEntry, char *str);
	 BOOL Add(char *sSection, char *sEntry, int value);
	 int  Get(char *sSection, char *sEntry, char *str, char *sDefault="");
	 UINT Get(char *sSection, char *sEntry, int iDefault=0);

private:
	 char _sFileName[MAX_FILENAME_LENGTH];
};

#endif // __CPROFILE_H
