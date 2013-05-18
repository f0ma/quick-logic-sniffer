#include "oslproducer.h"


void OSLProducer::run()
{
    header.clear();
    outputQueue.clear();

    header << QString(";Rate: %1\r\n").arg(frq);
    if(!micronasmode)header << QString(";Channels: %1\r\n").arg(channals);
    else header << QString(";Channels: 2\r\n");
    if(lptmode) header << QString(";EnabledChannels: 63743\r\n");
    qint64 currentword;
    qint64 oldword;
    qint64 cd,cd1,cd2;
    QString sn;

    qint64 mask = 0xFF00;
    if (lptmode) mask = 0xF800;


   for(int i =0;i< data.size();i=i+channals/8)
   {


   switch(channals/8)
   {
   case 1:
   currentword = (unsigned char) data[i];
   break;

   case 2:
   cd = ((unsigned char) data[i+1]);
   currentword = ((cd<<8)&mask)+ ((unsigned char) data[i]);
   break;

   case 3:
   cd1 = ((unsigned char) data[i+2]);
   cd = ((unsigned char) data[i+1]);
   currentword = ((cd1<<16)&0xFF0000) + ((cd<<8)&0xFF00)+ ((unsigned char) data[i]);
   break;

   case 4:
   cd2 = ((unsigned char) data[i+3]);
   cd1 = ((unsigned char) data[i+2]);
   cd = ((unsigned char) data[i+1]);
   currentword = ((cd2<<24)&0xFF000000)+((cd1<<16)&0xFF0000)+((cd<<8)&0xFF00)+ ((unsigned char)data[i]);
   break;

   }

   if(currentword != oldword || i == data.size()-channals/8 || i ==0)
   {

       if(!micronasmode)
       {
           sn.setNum(currentword,16);
           outputQueue.enqueue(QString("%1@%2\r\n").arg(sn).arg(i/(channals/8)));
       }
       else
       {
           sn.setNum((currentword>>0xE)&3,16);
           outputQueue.enqueue(QString("%1@%2\r\n").arg(sn).arg(i/(channals/8)));
       }
   }

   oldword=currentword;
   }
}


void OSLProducer::produceData(QByteArray data,unsigned int channals,unsigned int frq,bool lptmode = false,bool micronasmode=false)
{
    this->data=data;
    this->channals=channals;
    this->frq=frq;
    this->lptmode=lptmode;
    this->micronasmode=micronasmode;

    this->start();
}



void OSLSaver::run()
{
    msleep(100);

    if(split == 0)
    {

    QString s;

    QFile outfile(filename);
    outfile.open(QIODevice::WriteOnly);
    Q_ASSERT(outfile.isOpen());

    foreach(s,p->header)
        outfile.write(s.toAscii());


    while (p->isRunning() || (!p->outputQueue.isEmpty()))
    {
        while (!p->outputQueue.isEmpty())
            outfile.write(p->outputQueue.dequeue().toAscii());
        sleep(1);
    }

    outfile.close();
    }
    else
    {
        int part = 0;
        int lastn = 0;

        QString mfilename = filename.replace(".ols","-%1.ols");//QString("%1.ols").arg(part, 4, 10, QChar('0'))));

        while(true)
        {
        QFile outfile(mfilename.arg(part, 4, 10, QChar('0')));
        qDebug() << outfile.fileName();
        outfile.open(QIODevice::WriteOnly);
        Q_ASSERT(outfile.isOpen());

        QString s;
        foreach(s,p->header)
            outfile.write(s.toAscii());


        int n=0;

        while ((p->isRunning() || (!p->outputQueue.isEmpty())) && n<split)
        {
            while ((!p->outputQueue.isEmpty()) && n<split)
            {
                outfile.write(p->outputQueue.dequeue().toAscii());
            n++;
            }
            sleep(1);
        }



        part++;

        outfile.close();

        if(!p->isRunning() && p->outputQueue.isEmpty()) break;
        }
    }
}

void OSLSaver::produceFile(OSLProducer* p,QString filename,int split=0)
{
    this->p = p;
    this->filename = filename;
    this->split = split;

    this->start();
}


