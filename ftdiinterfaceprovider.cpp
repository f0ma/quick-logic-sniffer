#include "ftdiinterfaceprovider.h"

FtdiInterfaceProvider::FtdiInterfaceProvider()
{
#ifdef Q_WS_WIN
    lib.setFileName("ftd2xx.dll");
#else
    lib.setFileName("./libftd2xx");
#endif
    initOk = false;
}

int FtdiInterfaceProvider::load()
{
    if(initOk) return 1;

    lib.load();

    if(!lib.isLoaded())
    {
        initOk = false;
        qDebug() << lib.errorString();
        return 0;
    }

    CreateDeviceInfoList = (PrototypeCreateDeviceInfoList) lib.resolve("FT_CreateDeviceInfoList");
    GetDeviceInfoList = (PrototypeGetDeviceInfoList) lib.resolve("FT_GetDeviceInfoList");
    OpenEx = (PrototypeOpenEx) lib.resolve("FT_OpenEx");
    Close = (PrototypeClose) lib.resolve("FT_Close");
    Read = (PrototypeRead) lib.resolve("FT_Read");
    Write = (PrototypeWrite) lib.resolve("FT_Write");
    SetBaudRate = (PrototypeSetBaudRate) lib.resolve("FT_SetBaudRate");
    SetBitMode = (PrototypeSetBitMode) lib.resolve("FT_SetBitMode");

     return 1;
}

void FtdiInterfaceProvider::unload()
{

}

FtdiInterfaceProvider::~FtdiInterfaceProvider()
{

}
