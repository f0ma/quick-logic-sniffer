#include "lptrecorder.h"

LptRecorder::LptRecorder(QObject *parent) :
    QThread(parent)
{
    prov = new WinIOProvider();
    portBase=0x378;
    bufferSize = 1024;
    data = NULL;
    addr = 0;
    cancelRequared = false;
    micronasMode = false;
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
    return QByteArray::fromRawData((const char*)data,bufferSize);
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


void LptRecorder::loadLibrary()
{
    if(prov->load()==1) errorString = "";
    else errorString = "Can't load WinIO library!";
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

    Q_ASSERT(prov->load()==1);

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
    cancelRequared = false;
    this->start(QThread::HighPriority);

}

void LptRecorder::cancel()
{
    cancelRequared = true;
    this->wait();
    cancelRequared = false;
}

void LptRecorder::run()
{
    emit recordStarted();

    delete [] data;
    data = new unsigned char [bufferSize*2];

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
