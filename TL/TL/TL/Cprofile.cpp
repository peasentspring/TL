// CProfile.cpp

/*
 * $Id: Cprofile.cpp 4 2013-10-16 08:25:28Z STONERIDGE\mima $
 *
 */

#include "stdafx.h"	// Added to be compatible with MSVC

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "cprofile.h"

//// class C_PrivProfile //////////////////////////////////////////////////////

C_PrivProfile::C_PrivProfile(char *sFileName)
{
   _sFileName[127] = '\0';
   strncpy_s(_sFileName, sFileName, MAX_FILENAME_LENGTH);
}

BOOL C_PrivProfile::Add(char *sSection, char *sEntry, char *str)
{
  return WritePrivateProfileString(sSection,sEntry,str,_sFileName);
}

BOOL C_PrivProfile::Add(char *sSection, char *sEntry, int value)
{
  char str[128];

  if (_itoa_s(value, str, 128, 10) != 0)
  {
      (void)MessageBox(NULL, "C_PrivProfile::Add()\n\n"
         "_itoa_s() failed to convert int to string.",
         "SREAPDiag Error",
         MB_OK | MB_SETFOREGROUND | MB_ICONWARNING);
  }
  return WritePrivateProfileString(sSection,sEntry,str,_sFileName);
}

int C_PrivProfile::Get(char *sSection, char *sEntry, char *str, char *sDefault)
{
  return GetPrivateProfileString(sSection,sEntry,sDefault,str,128,_sFileName);
}

UINT C_PrivProfile::Get(char *sSection, char *sEntry, int iDefault)
{
  return GetPrivateProfileInt(sSection,sEntry,iDefault,_sFileName);
}
