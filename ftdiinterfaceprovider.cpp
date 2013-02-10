#include "ftdiinterfaceprovider.h"

FtdiInterfaceProvider::FtdiInterfaceProvider()
{
    lib.setFileName("ftd2xx.dll");
    initOk = false;
}

int FtdiInterfaceProvider::load()
{
    if(initOk) return 1;

    lib.load();

    if(!lib.isLoaded())
    {
        initOk = false;
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

}

void FtdiInterfaceProvider::unload()
{

}

FtdiInterfaceProvider::~FtdiInterfaceProvider()
{

}
