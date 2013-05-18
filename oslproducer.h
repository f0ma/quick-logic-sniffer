#ifndef OSLPRODUCER_H
#define OSLPRODUCER_H

#include <QThread>
#include <QQueue>
#include <QVector>
#include <QFile>
#include <QDebug>

class OSLProducer : public QThread
{
    void run();


    QByteArray data;
    unsigned int channals;
    unsigned int frq;
    bool lptmode;
    bool micronasmode;

    public:
    QQueue<QString> outputQueue;
    QVector<QString> header;
    
    void produceData(QByteArray data,unsigned int channals,unsigned int frq,bool lptmode,bool micronasmode);

};

class OSLSaver : public QThread
{
    void run();

    OSLProducer* p;
    QString filename;
    int split;

    public:
    void produceFile(OSLProducer* p,QString filename,int split);

};

#endif // OSLPRODUCER_H
