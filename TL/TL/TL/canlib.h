/*
**                         Copyright 1994-1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** $Id: canlib.h 4 2013-10-16 08:25:28Z STONERIDGE\mima $
**
*/
#ifndef _CANLIB_H_
#define _CANLIB_H_

#include "std.h"
#include "canstat.h"
#include "predef.h"
#include "canevt.h"
#include <stdlib.h>

#ifdef _WINDOWS
  #include <windows.h>
  #ifdef __WIN32__
    #include <winnt.h>
  #endif // __WIN32__
#endif // _WINDOWS

typedef int canHandle;

#define canINVALID_HANDLE      (-1)

#define WM__CANLIB              (WM_USER+16354)


#define canCIRCUIT_ANY          -1          // Any circuit will do
#define canCARD_ANY             -1          // Any card will do
#define canCHANNEL_ANY          -1          // Any channel will do

// Flags for canOpen
// 0x01, 0x02, 0x04 are obsolete and reserved.
#define canWANT_EXCLUSIVE       0x08
#define canWANT_EXTENDED        0x10   


// Flags for canAccept
#define canFILTER_ACCEPT        1
#define canFILTER_REJECT        2
#define canFILTER_SET_CODE_STD  3
#define canFILTER_SET_MASK_STD  4
#define canFILTER_SET_CODE_EXT  5
#define canFILTER_SET_MASK_EXT  6

#define canFILTER_NULL_MASK     0L


//
// CAN driver types - not all are supported on all cards.
//
#define canDRIVER_NORMAL           4
#define canDRIVER_SILENT           1
#define canDRIVER_SELFRECEPTION    8
#define canDRIVER_OFF              0
// 2,3,5,6,7 are reserved values for compability reasons.

/*
** Common bus speeds. Used in canSetBusParams.
** The values are translated in canlib, canTranslateBaud().
*/
#define BAUD_1M        (-1)
#define BAUD_500K      (-2)
#define BAUD_250K      (-3)
#define BAUD_125K      (-4)
#define BAUD_100K      (-5)
#define BAUD_62K       (-6)
#define BAUD_50K       (-7)

/*
** IOCTL types
*/
#define canIOCTL_PREFER_EXT             1
#define canIOCTL_PREFER_STD             2
// 3,4 reserved.
#define canIOCTL_CLEAR_ERROR_COUNTERS   5
#define canIOCTL_SET_TIMER_SCALE        6
#define canIOCTL_SET_TXACK              7


#define CANID_METAMSG  (-1L)        // Like msgs containing bus status changes
#define CANID_WILDCARD (-2L)        // We don't care or don't know

// Retained for compability.
typedef void* (*canMemoryAllocator)  (size_t size);
typedef void  (*canMemoryDeallocator)(void* p);

typedef struct tagCanHWDescr {
   int                    circuitType;
   int                    cardType;
   int                    channel;
} canHWDescr;

typedef struct tagCanSWDescr {
   unsigned int           rxBufSize;
   unsigned int           txBufSize;
   canMemoryAllocator     alloc;
   canMemoryDeallocator   deAlloc;
} canSWDescr;

#ifdef __WIN32__
typedef struct {
   unsigned int size;
   unsigned int rxBufSize;
   unsigned int txBufSize;
} canSWDescriptorEx;
#endif

//
// Define CANLIBAPI unless it's done already.
// (canlib.c provides its own definitions of CANLIBAPI, DLLIMPORT
// and DLLEXPORT before including this file.)
//
#ifndef CANLIBAPI
#   if defined(__WIN32__)
#      if defined(__BORLANDC__)
#         define CANLIBAPI __stdcall
#         define DLLIMPORT 
#         define DLLEXPORT _export
#      elif defined(_MSC_VER)
#         define CANLIBAPI __stdcall
#         define DLLIMPORT __declspec(dllimport)
#         define DLLEXPORT __declspec(dllexport)
#      endif
#   elif defined(_WINDOWS)
       // Win16
#      define CANLIBAPI _export _far _pascal
#   else
       // DOS
#      define CANLIBAPI _far
#   endif
#endif

#ifndef FAR
#   if defined(__WIN32__)
#       define FAR
#   else
#       define FAR _far
#   endif
#endif

// "extern" - definitions of all routines in canlib
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WINDOWS
int WEP(int nParameter);
#endif

void CANLIBAPI canInitializeLibrary(void);

canStatus CANLIBAPI canLocateHardware(void);

int CANLIBAPI canOpen(const canHWDescr FAR * const hwdescr,
                      const canSWDescr FAR * const swdescr,
                      const unsigned int flags);

canStatus CANLIBAPI canClose(const int handle);

canStatus CANLIBAPI canBusOn(const int handle);

canStatus CANLIBAPI canBusOff(const int handle);

canStatus CANLIBAPI canSetBusParams(const int handle,
                           long freq,
                           unsigned int tseg1,
                           unsigned int tseg2,
                           unsigned int sjw,
                           unsigned int noSamp,
                           unsigned int syncmode);

canStatus CANLIBAPI canGetBusParams(const int handle,
                              long FAR * freq,
                              unsigned int FAR * tseg1,
                              unsigned int FAR * tseg2,
                              unsigned int FAR * sjw,
                              unsigned int FAR * noSamp,
                              unsigned int FAR * syncmode);

canStatus CANLIBAPI canSetBusOutputControl(const int handle,
                                     const unsigned int drivertype);

canStatus CANLIBAPI canGetBusOutputControl(const int handle,
                                     unsigned int FAR * drivertype);

canStatus CANLIBAPI canAccept(const int handle,
                        const long envelope,
                        const unsigned int flag);

canStatus CANLIBAPI canReadStatus(const int handle,
                            unsigned long FAR * const flags);

canStatus CANLIBAPI canReadErrorCounters(int handle,
                               unsigned int FAR * txErr,
                               unsigned int FAR * rxErr,
                               unsigned int FAR * ovErr);

canStatus CANLIBAPI canWrite(int handle, long id, void FAR * msg,
                       unsigned int dlc, unsigned int flag);

canStatus CANLIBAPI canWriteSync(int handle, unsigned long timeout);

canStatus CANLIBAPI canRead(int handle,
                      long FAR * id,
                      void FAR * msg,
                      unsigned int FAR * dlc,
                      unsigned int FAR * flag,
                      unsigned long FAR * time);

canStatus CANLIBAPI canReadWait(int handle,
                          long FAR * id,
                          void FAR * msg,
                          unsigned int FAR * dlc,
                          unsigned int FAR * flag,
                          unsigned long FAR * time,
                          unsigned long timeout);

canStatus CANLIBAPI canReadSpecific(int handle, long id, void FAR * msg,
                              unsigned int FAR * dlc, unsigned int FAR * flag,
                              unsigned long FAR * time);

canStatus CANLIBAPI canReadSync(int handle, unsigned long timeout);

canStatus CANLIBAPI canReadSyncSpecific(int handle, long id, unsigned long timeout);

canStatus CANLIBAPI canReadSpecificSkip(int hnd,
                                  long id,
                                  void FAR * msg,
                                  unsigned int FAR * dlc,
                                  unsigned int FAR * flag,
                                  unsigned long FAR * time);

canStatus CANLIBAPI canSetNotify(int handle, HWND aHWnd, unsigned int aNotifyFlags);

canStatus CANLIBAPI canTranslateBaud(long FAR * const freq,
                               unsigned int FAR * const tseg1,
                               unsigned int FAR * const tseg2,
                               unsigned int FAR * const sjw,
                               unsigned int FAR * const nosamp,
                               unsigned int FAR * const syncMode);

canStatus CANLIBAPI canGetErrorText(canStatus err, char FAR * buf, unsigned int bufsiz);

unsigned short CANLIBAPI canGetVersion(void);

canStatus CANLIBAPI canGetCircuits(int FAR * context,
                             char FAR * name,
                             char FAR * vendor,
                             char FAR * version,
                             unsigned int FAR * cardtype,
                             unsigned int FAR * circtype,
                             unsigned int FAR * channel);

canStatus CANLIBAPI canIoCtl(int handle, unsigned int func,
                       void FAR * buf, unsigned int buflen);

#ifdef __WIN32__
unsigned long CANLIBAPI canReadTimer(int hnd);
#else
unsigned long CANLIBAPI canReadTimer(void);
#endif

//
// The following routines are found in canlib32 only.
//
#ifdef __WIN32__
int CANLIBAPI canOpenChannel(int channel, int flags);

canStatus CANLIBAPI canGetNumberOfChannels(int * channelCount);

canStatus CANLIBAPI canGetChannelData(int channel, int item, void *buffer, size_t bufsize);

#define canCHANNELDATA_CHANNEL_CAP              1
#define canCHANNELDATA_TRANS_CAP                2
#define canCHANNELDATA_CHANNEL_FLAGS            3   // available, etc
#define canCHANNELDATA_CARD_TYPE                4   // canHWTYPE_xxx
#define canCHANNELDATA_CARD_NUMBER              5   // Number in machine, 0,1,...
#define canCHANNELDATA_CHAN_NO_ON_CARD          6
#define canCHANNELDATA_CARD_SERIAL_NO           7
#define canCHANNELDATA_TRANS_SERIAL_NO          8
#define canCHANNELDATA_CARD_FIRMWARE_REV        9
#define canCHANNELDATA_CARD_HARDWARE_REV        10
#define canCHANNELDATA_CARD_UPC_NO              11
#define canCHANNELDATA_TRANS_UPC_NO             12
#define canCHANNELDATA_CHANNEL_NAME             13

// channelFlags in canChannelData
#define canCHANNEL_IS_EXCLUSIVE         0x0001
#define canCHANNEL_IS_OPEN              0x0002

// For canOpen(), canOpenChannel()
#define canWANT_VIRTUAL                 0x0020

// Hardware types.
#define canHWTYPE_NONE         0        // Unknown
#define canHWTYPE_VIRTUAL      1        // Virtual channel.
#define canHWTYPE_LAPCAN       2        // LAPcan
#define canHWTYPE_CANPARI      3        // CANpari
#define canHWTYPE_PCCAN        8        // PCcan-X
#define canHWTYPE_PCICAN       9        // PCIcan-X

// Channel capabilities.
#define canCHANNEL_CAP_EXTENDED_CAN         0x00000001L
#define canCHANNEL_CAP_BUS_STATISTICS       0x00000002L
#define canCHANNEL_CAP_ERROR_COUNTERS       0x00000004L
#define canCHANNEL_CAP_CAN_DIAGNOSTICS      0x00000008L
#define canCHANNEL_CAP_GENERATE_ERROR       0x00000010L
#define canCHANNEL_CAP_GENERATE_OVERLOAD    0x00000020L
#define canCHANNEL_CAP_TXREQUEST            0x00000040L
#define canCHANNEL_CAP_TXACKNOWLEDGE        0x00000080L
#define canCHANNEL_CAP_VIRTUAL              0x00010000L

// Driver (transceiver) capabilities
#define canDRIVER_CAP_HIGHSPEED             0x00000001L

#define canIOCTL_GET_RX_BUFFER_LEVEL            8
#define canIOCTL_GET_TX_BUFFER_LEVEL            9
#define canIOCTL_FLUSH_RX_BUFFER                10
#define canIOCTL_FLUSH_TX_BUFFER                11
#define canIOCTL_GET_TIMER_SCALE                12
#define canIOCTL_SET_TXRQ                       13
#define canIOCTL_GET_EVENTHANDLE                14
#define canIOCTL_SET_BYPASS_MODE                15
#define canIOCTL_SET_WAKEUP                     16
#define canIOCTL_GET_DRIVERHANDLE               17
#define canIOCTL_MAP_RXQUEUE                    18


canStatus CANLIBAPI canReadEvent(int hnd, CanEvent *event);

canStatus CANLIBAPI canWaitForEvent(int hnd, DWORD timeout);

canStatus CANLIBAPI canSetBusParamsC200(int hnd, BYTE btr0, BYTE btr1);

canStatus CANLIBAPI canSetDriverMode(int hnd, int lineMode, int resNet);



// Not implemented.
void CANLIBAPI canSetDebug(int d);

// Not implemented.
canStatus CANLIBAPI canSetNotifyEx(int handle, HANDLE event, unsigned int flags);

// Not implemented.
canStatus CANLIBAPI canSetTimer(int hnd, DWORD interval, DWORD flags);
#define canTIMER_CYCLIC             0x01
#define canTIMER_EXPENSIVE          0x02

// Not implemented.
typedef struct canBusStatistics {
    int qqqblabla;
} canBusStatistics;
// Not implemented.
canStatus CANLIBAPI canGetBusStatistics(int hnd, canBusStatistics *stat, size_t buf);

// Not implemented.
int CANLIBAPI canSplitHandle(int hnd, int channel);

// Not implemented.
int CANLIBAPI canOpenMultiple(DWORD bitmask, int flags);

#endif



#ifdef __cplusplus
}
#endif

#include "obsolete.h"

#endif



