#include "ftdirecorder.h"

FTDIRecorder::FTDIRecorder(QObject *parent) :
    QThread(parent)
{
    status = "Idle";
    data = NULL;
    bufferSize = 0;
    prov = new FtdiInterfaceProvider();
}

void FTDIRecorder::run()
{

}

void FTDIRecorder::loadLibrary()
{
    if(prov->load()==1) status = "" ;
    else  status = "Can't load FTDI library!";
}

QStringList FTDIRecorder::getDevicesList()
{
    DWORD numDevs;

    if(prov->load()!=1) return QStringList();

    if (prov->CreateDeviceInfoList (&numDevs) == 0 && numDevs >0)
    {
        FT_DEVICE_LIST_INFO_NODE * devInfo = (FT_DEVICE_LIST_INFO_NODE*) malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
        Q_ASSERT(prov->GetDeviceInfoList(devInfo,&numDevs)==0);

        QStringList r;

        for(int i = 0;i<numDevs;i++)
        {
                r.append(devInfo[i].SerialNumber);
        }

        return r;
    }
     return    QStringList();
}

void FTDIRecorder::setEnabledDevices(QList<bool>)
{

}

void FTDIRecorder::setSampleCount(unsigned int sz)
{

}

void FTDIRecorder::setSpeed(int speed)
{

}

FTDIRecorder::~FTDIRecorder()
{

}

unsigned char FTDIRecorder::progress()
{
    return 0;
}

QByteArray FTDIRecorder::getData()
{
    return QByteArray((signed)data,bufferSize);
}

QString FTDIRecorder::getStatus()
{
    if (status == "") return "OK";
    return status;
}

bool FTDIRecorder::canRun()
{
    return status == "";
}

void FTDIRecorder::startRecord()
{

}

void FTDIRecorder::cancel()
{

}
