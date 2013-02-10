#ifndef FTDIRECORDER_H
#define FTDIRECORDER_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QVector>
#include "ftdiinterfaceprovider.h"

class FTDIRecorder : public QThread
{
    Q_OBJECT

    QString status;
    unsigned char * data;
    unsigned int bufferSize;
    FtdiInterfaceProvider * prov;
    QStringList enabledDevice;
    unsigned int addr;
    int enabledDeviceCount;
    unsigned int speed;
    bool cancelRequared;


public:
    explicit FTDIRecorder(QObject *parent = 0);
    virtual ~FTDIRecorder();
    unsigned char progress();
    QByteArray getData();
    QString getStatus();
    bool canRun();
    int getChannalsCount();

    void run();

signals:
    void recordStarted();
    void recordEnded();

public slots:
    void loadLibrary();
    QStringList getDevicesList();
    void setEnabledDevices(QStringList l);
    void setSampleCount(unsigned int sz);
    void setSpeed(int speed);

    void startRecord();
    void cancel();
};

#endif // FTDIRECORDER_H
