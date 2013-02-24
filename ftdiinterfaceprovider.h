#ifndef FTDIINTERFACEPROVIDER_H
#define FTDIINTERFACEPROVIDER_H

#include <QObject>
#include <QLibrary>
#include <QDebug>

#ifdef Q_WS_WIN
#include <windows.h>
#else
typedef unsigned int UINT;
typedef char* LPCTSTR;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned int* PDWORD;
typedef unsigned int* LPDWORD;
typedef void*	PVOID;
typedef void*	LPVOID;
#endif

typedef void*	FT_HANDLE;
typedef ULONG	FT_STATUS;

typedef struct {
    ULONG Flags;
    ULONG Type;
    ULONG ID;
    DWORD LocId;
    char SerialNumber[16];
    char Description[64];
    FT_HANDLE ftHandle;
} FT_DEVICE_LIST_INFO_NODE;

/*
FT_STATUS FT_CreateDeviceInfoList(LPDWORD lpdwNumDevs);
FT_DEVICE_LIST_INFO_NODE * FT_GetDeviceInfoList(FT_DEVICE_LIST_INFO_NODE *pDest,LPDWORD lpdwNumDevs);

FT_STATUS FT_OpenEx(PVOID pArg1,DWORD Flags,FT_HANDLE *pHandle);
FT_STATUS FT_Close(FT_HANDLE ftHandle);

FT_STATUS FT_Read(FT_HANDLE ftHandle,LPVOID lpBuffer,DWORD dwBytesToRead,LPDWORD lpBytesReturned);
FT_STATUS FT_Write(FT_HANDLE ftHandle, LPVOID lpBuffer,DWORD dwBytesToWrite,LPDWORD lpBytesWritten);

FT_STATUS FT_SetBaudRate(FT_HANDLE ftHandle,ULONG BaudRate);
FT_STATUS FT_SetBitMode(FT_HANDLE ftHandle,UCHAR ucMask,UCHAR ucEnable);
*/


class FtdiInterfaceProvider
{

    QLibrary lib;
    bool initOk;
public:

#ifdef Q_WS_WIN
#define IMPORTED_FROM_LIBRARY __stdcall
#else
#define IMPORTED_FROM_LIBRARY 
#endif

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeCreateDeviceInfoList)(LPDWORD lpdwNumDevs);
    typedef FT_DEVICE_LIST_INFO_NODE * (IMPORTED_FROM_LIBRARY *PrototypeGetDeviceInfoList)(FT_DEVICE_LIST_INFO_NODE *pDest,LPDWORD lpdwNumDevs);

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeOpenEx)(PVOID pArg1,DWORD Flags,FT_HANDLE *pHandle);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeClose)(FT_HANDLE ftHandle);

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeRead)(FT_HANDLE ftHandle,LPVOID lpBuffer,DWORD dwBytesToRead,LPDWORD lpBytesReturned);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeWrite)(FT_HANDLE ftHandle, LPVOID lpBuffer,DWORD dwBytesToWrite,LPDWORD lpBytesWritten);

    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetBaudRate)(FT_HANDLE ftHandle,ULONG BaudRate);
    typedef FT_STATUS (IMPORTED_FROM_LIBRARY *PrototypeSetBitMode)(FT_HANDLE ftHandle,UCHAR ucMask,UCHAR ucEnable);

    PrototypeCreateDeviceInfoList CreateDeviceInfoList;
    PrototypeGetDeviceInfoList GetDeviceInfoList;
    PrototypeOpenEx OpenEx;
    PrototypeClose Close;
    PrototypeRead Read;
    PrototypeWrite Write;
    PrototypeSetBaudRate SetBaudRate;
    PrototypeSetBitMode SetBitMode;

    FtdiInterfaceProvider();
    int load();
    void unload();
    ~FtdiInterfaceProvider();
};

#endif // FTDIINTERFACEPROVIDER_H
