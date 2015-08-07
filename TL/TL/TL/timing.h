
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TIMING_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TIMING_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef TIMING_EXPORTS
#define TIMING_API __declspec(dllexport)
#else
#define TIMING_API __declspec(dllimport)
#endif

#define IDM_TIMER            509

TIMING_API void CALLBACK TimerCallBack(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);

