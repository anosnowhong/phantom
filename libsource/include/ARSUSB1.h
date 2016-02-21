//---------------------------------------ARSUSB1.h------------------------------------------------------
//
//						ARSUSB1.h : header file for ISA/PCMCIA to USB API 1.0
//						Version 1.2
//---------------------------------------------------------------------------------------------

#pragma once
#include <windows.h>

#pragma pack( push, USB_union, 1 )

struct _OUTP 
{	
	BYTE bReserved;
	WORD wReserved;
	BYTE bData;
};

struct _OUTP_W 
{  
	BYTE bReserved;
	WORD wReserved;
	WORD wData;
};

struct _OUTP_COND 
{	
	BYTE bReserved;
	WORD wReserved;
	WORD wReserved1;
	BYTE bReserved1;
	BYTE bData;
};

struct _OUTP_W_COND 
{  
	BYTE bReserved;
	WORD wReserved;
	WORD wReserved1;
	BYTE bReserved1;
	WORD wData;
};

struct _IO_WRITE_FIFO_W 
{  
	BYTE bReserved;
	WORD wReserved;
	WORD wReserved1;
	BYTE bReserved1;
	WORD wData[32765];
};

struct _IO_WRITE_FIFO 
{  
	BYTE bReserved;
	WORD wReserved;
	WORD wReserved1;
	BYTE bData[65531];
};

struct _W_ARR
{
	WORD wAddr;
	BYTE bData;
};

struct _IO_WRITE
{  
	BYTE bReserved;
	WORD wReserved;
	struct _W_ARR AD[21503];  
};

struct _W_ARR1
{
	struct _W_ARR AD[21];  
	BYTE bReserved;
};

struct _IO_WRITE1
{  
	struct _W_ARR1 Pack[1024];  
};

struct _W_ARR_W
{
	WORD wAddr;
	WORD wData;
};

struct _IO_WRITE_W
{  
	BYTE bReserved;
	WORD wReserved;
	BYTE bReserved1;
	struct _W_ARR_W AD[16383];  
};

struct _MEM_WRITE_W 
{  
	BYTE bReserved;
	WORD wReserved;
	WORD wReserved1;
	BYTE bReserved1;
	WORD wData[32765];
};

struct _MEM_WRITE 
{  
	BYTE bReserved;
	WORD wReserved;
	WORD wReserved1;
	BYTE bReserved1;
	BYTE bData[65530];
};

struct _READ_B 
{  
	BYTE bData[65536];
};

struct _READ_W 
{  
	WORD wData[32768];
};

union _USB_PACK 
{
	struct _OUTP_W cOutPW;  
	struct _OUTP cOutP;  
	struct _OUTP_COND cOutPCond;
	struct _OUTP_W_COND cOutPWCond;
	struct _IO_WRITE_FIFO_W cOutFIFOW;
	struct _IO_WRITE_FIFO cOutFIFO;
	struct _MEM_WRITE_W cMemWrW;  
	struct _MEM_WRITE cMemWr;  
	struct _READ_B ReadB;
	struct _READ_W ReadW;
	struct _IO_WRITE_W cOutArrW;
	struct _IO_WRITE cOutArr;
	struct _IO_WRITE1 cOutArr1;
};
//#define IMPLICIT_DLL_LINKING
#pragma pack( pop, USB_union )

#ifdef IMPLICIT_DLL_LINKING

__declspec(dllimport) WORD InP(WORD port);
__declspec(dllimport) WORD InPW(WORD port);
__declspec(dllimport) WORD OutP(WORD port, BYTE data);
__declspec(dllimport) WORD OutPW(WORD port, WORD data);
__declspec(dllimport) WORD InFIFO(WORD port, WORD length);
__declspec(dllimport) WORD InFIFOW(WORD port, WORD length);
__declspec(dllimport) WORD OutFIFO(WORD port, WORD length);
__declspec(dllimport) WORD OutFIFOW(WORD port, WORD length);
__declspec(dllimport) WORD MemRd(DWORD addr, WORD length);
__declspec(dllimport) WORD MemRdW(DWORD addr, WORD length);
__declspec(dllimport) WORD MemWr(DWORD addr, WORD length);
__declspec(dllimport) WORD MemWrW(DWORD addr, WORD length);
__declspec(dllimport) WORD USBInit(union _USB_PACK * apUSB);
__declspec(dllimport) WORD DrvOpen(char * name);
__declspec(dllimport) WORD DrvClose();
__declspec(dllimport) WORD USBLastErr();
__declspec(dllimport) WORD USBConnected();
__declspec(dllimport) WORD OutArray(WORD length);
__declspec(dllimport) WORD OutArrayW(WORD length);
__declspec(dllimport) WORD AttrMemRd(DWORD addr, WORD length);
__declspec(dllimport) WORD AttrMemWr(DWORD addr, WORD length);
__declspec(dllimport) WORD InPCond(WORD port, WORD StPort, BYTE Mask);
__declspec(dllimport) WORD InPWCond(WORD port, WORD StPort, BYTE Mask);
__declspec(dllimport) WORD OutPCond(WORD port, BYTE data, WORD StPort, BYTE Mask);
__declspec(dllimport) WORD OutPWCond(WORD port, WORD data, WORD StPort, BYTE Mask);
__declspec(dllimport) WORD TimeOut(WORD NewTimeOut, BYTE bCondModeBsy);
__declspec(dllimport) WORD SNumberRd();

#else

typedef WORD (*InP_)(WORD port);
typedef WORD (*InPW_)(WORD port);
typedef WORD (*OutP_)(WORD port, BYTE data);
typedef WORD (*OutPW_)(WORD port, WORD data);
typedef WORD (*InFIFO_)(WORD port, WORD length);
typedef WORD (*InFIFOW_)(WORD port, WORD length);
typedef WORD (*OutFIFO_)(WORD port, WORD length);
typedef WORD (*OutFIFOW_)(WORD port, WORD length);
typedef WORD (*MemRd_)(DWORD addr, WORD length);
typedef WORD (*MemRdW_)(DWORD addr, WORD length);
typedef WORD (*MemWr_)(DWORD addr, WORD length);
typedef WORD (*MemWrW_)(DWORD addr, WORD length);
typedef WORD (*USBInit_)(union _USB_PACK * apUSB);
typedef WORD (*DrvOpen_)(char * name);
typedef WORD (*DrvClose_)();
typedef WORD (*USBLastErr_)();
typedef WORD (*USBConnected_)();
typedef WORD (*OutArray_)(WORD length);
typedef WORD (*OutArrayW_)(WORD length);
typedef WORD (*AttrMemRd_)(DWORD addr, WORD length);
typedef WORD (*AttrMemWr_)(DWORD addr, WORD length);
typedef WORD (*InPCond_)(WORD port, WORD StPort, BYTE Mask);
typedef WORD (*InPWCond_)(WORD port, WORD StPort, BYTE Mask);
typedef WORD (*OutPCond_)(WORD port, BYTE data, WORD StPort, BYTE Mask);
typedef WORD (*OutPWCond_)(WORD port, WORD data, WORD StPort, BYTE Mask);
typedef WORD (*TimeOut_)(WORD NewTimeOut, BYTE bCondModeBsy);
typedef WORD (*EEPROMRd_)(DWORD addr, WORD length);
typedef WORD (*EEPROMWr_)(DWORD addr, WORD length);
typedef WORD (*SNumberRd_)();

extern InP_ InP;
extern InPW_ InPW;
extern OutP_ OutP;
extern OutPW_ OutPW;
extern InFIFO_ InFIFO;
extern InFIFOW_ InFIFOW;
extern OutFIFO_ OutFIFO;
extern OutFIFOW_ OutFIFOW;
extern MemRd_ MemRd;
extern MemRdW_ MemRdW;
extern MemWr_ MemWr;
extern MemWrW_ MemWrW;
extern USBInit_ USBInit;
extern DrvOpen_ DrvOpen;
extern DrvClose_ DrvClose;
extern USBLastErr_ USBLastErr;
extern USBConnected_ USBConnected;
extern OutArray_ OutArray;
extern OutArrayW_ OutArrayW;
extern AttrMemRd_ AttrMemRd;
extern AttrMemWr_ AttrMemWr;
extern InPCond_ InPCond;
extern InPWCond_ InPWCond;
extern OutPCond_ OutPCond;
extern OutPWCond_ OutPWCond;
extern TimeOut_ TimeOut;
extern EEPROMRd_ EEPROMRd;
extern EEPROMWr_ EEPROMWr;
extern SNumberRd_ SNumberRd;

LoadARSUSBdll();

#endif // ndef IMPLICIT_DLL_LINKING

  

