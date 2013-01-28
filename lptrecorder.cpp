#include "lptrecorder.h"

LptRecorder::LptRecorder(QObject *parent) :
    QThread(parent)
{
    WinIOProvider * prov = new WinIOProvider();
    unsigned int portBase=0x378;
    unsigned int bufferSize = 1024;
    unsigned char * data = NULL;
    unsigned int addr = 0;
    bool cancelRequared = false;
    bool micronasMode = false;
    errorString = "Yet not ready";
}


LptRecorder::~LptRecorder()
{
    delete data;
    delete prov;
}

unsigned char LptRecorder::progress()
{
    return ((double)addr)/((double)bufferSize)*100;
}

QByteArray LptRecorder::getData()
{
    return QByteArray((signed)data,bufferSize);
}

QString LptRecorder::getStatus()
{
    if (errorString == "") return "Ready";
    else return errorString;
}

bool LptRecorder::canRun()
{
    return errorString == "";
}


void recordStarted();
void recordEnded();

void LptRecorder::loadLibrary()
{

}

void LptRecorder::setPortNumber(unsigned int bn)
{
    switch(bn)
    {
    case 1 :
           portBase = 0x378;
           break;
    case 2 :
           portBase = 0x278;
           break;
    case 3 :
           portBase = 0x478;
           break;
    default:
        Q_ASSERT(false);
    }


}

void LptRecorder::setSampleCount(unsigned int sz)
{
    bufferSize = 2*sz;
}

void LptRecorder::setMode(bool m)
{
    micronasMode = m;

    DWORD conf = 0;

    prov->SetPortVal(portBase+0x402,0x35,1);
    prov->GetPortVal(portBase+2,&conf,1);

    if(!micronasMode)
    {
        conf  = (conf & 0x0f) +0x20;
        prov->SetPortVal(portBase+2,conf,1);
    }
    else
    {
        conf  = (conf & 0x0f);
        prov->SetPortVal(portBase+2,conf,1);
        prov->SetPortVal(portBase,0,1);
    }

}

void LptRecorder::startRecord()
{
    setMode(micronasMode);

    this->start(QThread::HighPriority);

}

void LptRecorder::cancel()
{
    cancelRequared = true;
    this->wait();
}

void LptRecorder::run()
{
    emit recordStarted();

    addr = 0;

    while((cancelRequared == false) && (addr < bufferSize))
    {

        DWORD ret;
        prov->GetPortVal(portBase,&ret,2);
        unsigned char d = ret;
        unsigned char s = ret>>8;

        unsigned char inv = ~s;

        s=s & 0x78;          //01111000
                             //I    UUU
        inv=inv & 0x80;      //10000000
                             //I    UUU

        s = s + inv;

        data[addr*2] = d;
        data[addr*2+1] = s;
        addr++;
    }

    emit recordEnded();

}
