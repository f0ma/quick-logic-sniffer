///////////////////////////////////////////////////////////////////////
//HardwareExpert. WinIO Library provider.
//Copyright (C) 2011  Stanislav (f0ma) Ivanov
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////

#include "winioprovider.h"

WinIOProvider::WinIOProvider()
{
lib.setFileName("winio32.dll");
initOk = false;
}

int WinIOProvider::load()
{
if(initOk) return 1;

lib.load();

if(!lib.isLoaded())
{
    initOk = false;
    return 0;
}

    InitializeWinIo = (PrototypeBoolVoid ) lib.resolve("InitializeWinIo");
    ShutdownWinIo = (PrototypeVoidVoid ) lib.resolve("ShutdownWinIo");
    GetPortVal = (PrototypeWordPdwordByte ) lib.resolve("GetPortVal");
    SetPortVal = (PrototypeWordDwordByte ) lib.resolve("SetPortVal");

    if (InitializeWinIo()==true)
    {
        initOk = true;
        return 1;
    }
    else
    {
        initOk = false;
        return -255;
    }


}

void WinIOProvider::unload()
{
    ShutdownWinIo();
}

void WinIOProvider::setPort(unsigned int port,unsigned char value)
{
    SetPortVal(port,value, 1);
}

unsigned char WinIOProvider::getPort(unsigned int port)
{
    DWORD ret;
    GetPortVal(port,&ret,1);
    return (int)ret;
}

WinIOProvider::~WinIOProvider()
{
    ShutdownWinIo();
}
