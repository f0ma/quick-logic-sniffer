#ifndef LPTRECORDER_H
#define LPTRECORDER_H

#include <QObject>
#include <QThread>
#include <QByteArray>
#include "winioprovider.h"

class LptRecorder : public QThread
{
    Q_OBJECT

    WinIOProvider * prov;
    unsigned int portBase;
    unsigned int bufferSize;
    unsigned char * data;
    unsigned int addr;
    bool cancelRequared;
    bool micronasMode;
    QString errorString;

    void run();

public:

    explicit LptRecorder(QObject *parent = 0);
    virtual ~LptRecorder();
    unsigned char progress();
    QByteArray getData();
    QString getStatus();
    bool canRun();

signals:
    void recordStarted();
    void recordEnded();

public slots:
    void loadLibrary();
    void setPortNumber(unsigned int bn);
    void setSampleCount(unsigned int sz);
    void setMode(bool m);
    void startRecord();
    void cancel();

};

#endif // LPTRECORDER_H


