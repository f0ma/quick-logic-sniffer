#include "ftdirecorder.h"

FTDIRecorder::FTDIRecorder(QObject *parent) :
    QThread(parent)
{
    status = "Idle";
    data = NULL;
    bufferSize = 0;
    prov = new FtdiInterfaceProvider();
    enabledDeviceCount = 0;
    cancelRequared = false;
}

void FTDIRecorder::run()
{
    if (enabledDeviceCount ==0) return;

    emit recordStarted();

    addr = 0;

    delete [] data;
    data = new unsigned char [bufferSize];

    int oneStepBufferSize = 128;
    unsigned char * buffer = new unsigned char [oneStepBufferSize];

    FT_STATUS ftStatus;
    QVector<FT_HANDLE> FTDevices;

    QString serial;

    foreach (serial,enabledDevice)
    {
        FT_HANDLE ftH;

        ftStatus = prov->OpenEx((void *)serial.toLocal8Bit().constData(),1,&ftH);
        ftStatus = prov->SetBitMode(ftH,0x00,0x00); // reset
        ftStatus = prov->SetBitMode(ftH,0x00,0x01); // async bit-bang
        ftStatus = prov->SetBaudRate(ftH,speed);

        FTDevices.append(ftH);
    }

    FT_HANDLE dev;


    while((cancelRequared == false) && (addr+oneStepBufferSize*enabledDeviceCount < bufferSize))
    {

    int chcount = 0;

    foreach (dev,FTDevices)
    {
        DWORD readed =0;
        ftStatus = prov->Read(dev,buffer,oneStepBufferSize,&readed);
        if(ftStatus != 0) cancelRequared = true;

        Q_ASSERT(readed == oneStepBufferSize);

        for(int i =0;i<oneStepBufferSize;i++)
        {
                  data[addr+enabledDeviceCount*i+chcount]=buffer[i];
        }
              chcount++;
    }

    addr = addr + oneStepBufferSize*enabledDeviceCount;

    }


    foreach (dev,FTDevices)
    {
        prov->Close(dev);
    }

    delete [] buffer;

    emit recordEnded();
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

void FTDIRecorder::setEnabledDevices(QStringList l)
{
    enabledDevice = l;
    enabledDeviceCount =l.length();
}

void FTDIRecorder::setSampleCount(unsigned int sz)
{
    bufferSize = sz*enabledDeviceCount;
}

void FTDIRecorder::setSpeed(int speed)
{
    this->speed = speed;
}

FTDIRecorder::~FTDIRecorder()
{

}

unsigned char FTDIRecorder::progress()
{
    return ((double)addr * enabledDeviceCount)/((double)bufferSize)*100;
}

QByteArray FTDIRecorder::getData()
{
    return QByteArray::fromRawData((const char*)data,bufferSize);
}

QString FTDIRecorder::getStatus()
{
    if (status == "") return "Ready";
    return status;
}

int FTDIRecorder::getChannalsCount()
{
    return enabledDeviceCount;
}

int FTDIRecorder::getSpeed()
{
    return speed;
}

bool FTDIRecorder::canRun()
{
    return status == "";
}

void FTDIRecorder::startRecord()
{
    cancelRequared = false;
    this->start(QThread::HighPriority);
}

void FTDIRecorder::cancel()
{
    cancelRequared = true;
    this->wait();
    cancelRequared = false;
}
