#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->swPages->setCurrentIndex(0);
    ftdirec = NULL;
    lptrec = NULL;
    progtimer = new QTimer(this);
    progtimer->setSingleShot(false);
    progtimer->setInterval(100);
    ui->cbVCDsave_Ftdi->setVisible(false);
    ui->cbVCDsave_Lpt->setVisible(false);
    progress = NULL;


    QSettings settings;

    ui->lePathToSave_Lpt->setText(settings.value("LaseFileName","").toString());
    ui->lePathToSave_Ftdi->setText(settings.value("LaseFileName","").toString());
    renew_filename("LPT");
    renew_filename("FTDI");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbStartNext_clicked()
{

   if (ui->rbLptMode->isChecked())
    {
        lptrec = new LptRecorder(this) ;
        ui->swPages->setCurrentIndex(2);
        connect(lptrec,SIGNAL(recordStarted()),this,SLOT(lpt_started()));
        connect(lptrec,SIGNAL(recordEnded()),this,SLOT(lpt_finished()));
        connect(progtimer,SIGNAL(timeout()),this,SLOT(lpt_progress()));
        update_lptrec_state();

    }

    if (ui->rbFtdiMode->isChecked())
    {
        ftdirec = new FTDIRecorder(this) ;
        ui->swPages->setCurrentIndex(1);
        connect(ftdirec,SIGNAL(recordStarted()),this,SLOT(ftdi_started()));
        connect(ftdirec,SIGNAL(recordEnded()),this,SLOT(ftdi_finished()));
        connect(progtimer,SIGNAL(timeout()),this,SLOT(ftdi_progress()));
        update_ftdirec_state();
    }


}

void MainWindow::update_lptrec_state()
{
    lptrec->loadLibrary();
    if (lptrec->canRun()) normal_mode_Lpt();
    else disabled_mode_Lpt();
    ui->lDriverState_Lpt->setText(lptrec->getStatus());
}

void MainWindow::update_ftdirec_state()
{
    ftdirec->loadLibrary();
    QStringList devs = ftdirec->getDevicesList();
    ui->cbEndpointList_Ftdi->clear();
    QString ser;

    if (devs.length() ==0)
    {
        disabled_mode_Ftdi();
        return;
    }

    foreach(ser,devs)
    {
    QListWidgetItem * item = new QListWidgetItem(ser);
    item->setCheckState(Qt::Checked);
    ui->cbEndpointList_Ftdi->addItem(item);
    }

    ui->lDriverState_Ftdi->setText(ftdirec->getStatus());
    if (ftdirec->canRun()) normal_mode_Ftdi();
    else disabled_mode_Ftdi();
}

void MainWindow::ftdi_progress()
{
    ui->progressBar_Ftdi->setValue(ftdirec->progress());
}

void MainWindow::lpt_progress()
{
    ui->progressBar_Lpt->setValue(lptrec->progress());
}


bool MainWindow::storeToOSL(QByteArray data,unsigned int channals,unsigned int frq,QString filename,bool lptmode = false,bool micronasmode=false,int split=0)
{

    OSLProducer * p = new OSLProducer();
    OSLSaver * s = new OSLSaver();
    p->produceData(data,channals,frq,lptmode,micronasmode);
    s->produceFile(p,filename,split);

    connect(s,SIGNAL(finished()),this,SLOT(endProgress()),Qt::QueuedConnection);

    return true;

}


void MainWindow::startProgress(QString label)
{
    delete progress;
    progress = new QProgressDialog (this);
    progress->setWindowModality(Qt::WindowModal);
    progress->setLabelText("Storing files...");
    progress->setCancelButton(0);
    progress->setRange(0,0);
    progress->setMinimumDuration(0);
    progress->show();
}

void MainWindow::endProgress()
{
    delete progress;
    progress = NULL;
}

void MainWindow::ftdi_finished()
{
    normal_mode_Ftdi();
    ui->progressBar_Ftdi->setValue(100);



    startProgress("Saving DAT file...");
    QFile f(ui->lePathToSave_Ftdi->text());
    if (!f.open(QIODevice::WriteOnly)) return;

    QByteArray br = ftdirec->getData();

    f.write(br);

    f.close();

    QFile df(ui->lePathToSave_Ftdi->text().replace(".dat",".diz"));
    if (!df.open(QIODevice::WriteOnly)) return;



    df.write((QString("Channals count: %1\r\nRecord Speed:%2\r\n").arg(ftdirec->getChannalsCount()*8).arg(ftdirec->getSpeed()*5)).toAscii());

    df.close();

    endProgress();

    if(ui->cbOSDsave_Ftdi->isChecked())
    {
       startProgress("Saving OSL file...");
       storeToOSL(br,ftdirec->getChannalsCount()*8,ftdirec->getSpeed()*5,ui->lePathToSave_Ftdi->text().replace(".dat",".ols"));
    }

    if(ui->cbOSDSplitsave_Ftdi->isChecked())
    {
       startProgress("Saving OSL-split file...");
       storeToOSL(br,ftdirec->getChannalsCount()*8,ftdirec->getSpeed()*5,ui->lePathToSave_Ftdi->text().replace(".dat",".ols"),false,false,100000);
    }



        renew_filename("FTDI");
}

void MainWindow::lpt_finished()
{
   normal_mode_Lpt();
   ui->progressBar_Lpt->setValue(0);

   startProgress("Saving DAT file...");
   QFile f(ui->lePathToSave_Lpt->text());
   if (!f.open(QIODevice::WriteOnly)) return;

   QByteArray br = lptrec->getData();

   f.write(br);

   f.close();
   endProgress();

   if(ui->cbOSDsave_Lpt->isChecked())
   {
       startProgress("Saving OSL file...");
       storeToOSL(br,16,400000,ui->lePathToSave_Lpt->text().replace(".dat",".ols"),true,ui->cbMicronasMode_LPT->isChecked());
   }


   if(ui->cbOSDSplitsave_Lpt->isChecked())
   {
       startProgress("Saving OSL-split file...");
       storeToOSL(br,16,400000,ui->lePathToSave_Lpt->text().replace(".dat",".ols"),true,ui->cbMicronasMode_LPT->isChecked(),100000);
   }


    renew_filename("LPT");


}

void MainWindow::renew_filename(QString s)
{
    QRegExp rx("(.*)-([0-9][0-9][0-9][0-9])");
    QString oldFn;

    if(s=="LPT")oldFn = ui->lePathToSave_Lpt->text();
    if(s=="FTDI")oldFn = ui->lePathToSave_Ftdi->text();

    if (oldFn == "") return;

    QString nameHead = oldFn.replace(".dat","");

    int num =0;

    if (rx.indexIn(nameHead,0) != -1) {
        nameHead = rx.cap(1);
        num = rx.cap(2).toInt()+1;
    }

    QString newFn;
    newFn = QString("%1-%2.dat").arg(nameHead).arg(num, 4, 10, QChar('0'));
    if(s=="LPT")ui->lePathToSave_Lpt->setText(newFn);
    if(s=="FTDI")ui->lePathToSave_Ftdi->setText(newFn);

    QSettings settings;
    settings.setValue("LaseFileName",newFn);
}

void MainWindow::ftdi_started()
{
    record_mode_Ftdi();
    ui->progressBar_Ftdi->setValue(0);
    QSettings settings;
    settings.setValue("LaseFileName",ui->lePathToSave_Ftdi->text());
}

void MainWindow::lpt_started()
{
    record_mode_Lpt();
    ui->progressBar_Lpt->setValue(0);
    QSettings settings;
    settings.setValue("LaseFileName",ui->lePathToSave_Lpt->text());
}

void MainWindow::on_pbRun_Ftdi_clicked()
{
    if(ui->lePathToSave_Ftdi->text().isEmpty())
    {
    QMessageBox::warning(this, tr("Quick data logger"),
                                tr("Enter the file name to save data"),
                                QMessageBox::Cancel);
    return;
}
//    ftdirec->setSpeed();
//    ftdirec->setsetBufferSize();

    int speeds [] = {300,600,1200,2400,4800,9600,14400,19200,38400,57600,115200,230400,460800,921600};

    QStringList ed;

    for (int i =0;i<ui->cbEndpointList_Ftdi->count();i++)
        if (ui->cbEndpointList_Ftdi->item(i)->checkState()==Qt::Checked)
            ed.append(ui->cbEndpointList_Ftdi->item(i)->text());

    ftdirec->setEnabledDevices(ed);
    ftdirec->setSpeed(speeds[ui->cbSpeed_Ftdi->currentIndex()]);

    unsigned int bs = 1024;

    switch(ui->cbBufferSize_Ftdi->currentIndex())
    {
        case 0:
            bs = 0xFFFF;
            break;
        case 1:
            bs = 4*0xFFFF;
            break;
        case 2:
            bs = 16*0xFFFF;
            break;
        case 3:
            bs =4*16*0xFFFF;
            break;
        case 4:
            bs =16*16*0xFFFF;
            break;
        case 5:
            bs =32*16*0xFFFF;
            break;
        case 6:
            bs =64*16*0xFFFF;
            break;
        default:
        Q_ASSERT(false);
    };
    ftdirec->setSampleCount(bs);
    ftdirec->startRecord();

}

void MainWindow::on_pbCancel_Ftdi_clicked()
{
    ftdirec->cancel();
}

void MainWindow::on_pbBrowse_Lpt_clicked()
{
    QString fn = QFileDialog::getSaveFileName(this,"Data file",ui->lePathToSave_Lpt->text(),"Data files (*.dat)");
    if (fn=="")return;
    if(fn.endsWith(".dat")==false)fn.append(".dat");
    ui->lePathToSave_Lpt->setText(fn);
    QSettings settings;
    settings.setValue("LaseFileName",fn);
}

void MainWindow::record_mode_Ftdi()
{
    ui->cbEndpointList_Ftdi->setEnabled(false);
    ui->cbBufferSize_Ftdi->setEnabled(false);
    ui->lePathToSave_Ftdi->setEnabled(false);
    ui->cbSpeed_Ftdi->setEnabled(false);
    ui->pbBrowse_Ftdi->setEnabled(false);
    ui->pbRun_Ftdi->setEnabled(false);
    ui->pbCancel_Ftdi->setEnabled(true);
    ui->cbVCDsave_Ftdi->setEnabled(false);
    ui->cbOSDsave_Ftdi->setEnabled(false);
    ui->cbOSDSplitsave_Ftdi->setEnabled(false);
    progtimer->start();
}

void MainWindow::normal_mode_Ftdi()
{
    ui->cbEndpointList_Ftdi->setEnabled(true);
    ui->cbBufferSize_Ftdi->setEnabled(true);
    ui->lePathToSave_Ftdi->setEnabled(true);
    ui->pbBrowse_Ftdi->setEnabled(true);
    ui->pbRun_Ftdi->setEnabled(true);
    ui->cbSpeed_Ftdi->setEnabled(true);
    ui->pbCancel_Ftdi->setEnabled(false);
    ui->cbVCDsave_Ftdi->setEnabled(true);
    ui->cbOSDsave_Ftdi->setEnabled(true);
    ui->cbOSDSplitsave_Ftdi->setEnabled(true);
    progtimer->stop();
}

void MainWindow::disabled_mode_Ftdi()
{
    record_mode_Ftdi();
    ui->pbCancel_Ftdi->setEnabled(false);
}

void MainWindow::record_mode_Lpt()
{
    ui->cbPortName_Lpt->setEnabled(false);
    ui->cbBufferSize_Lpt->setEnabled(false);
    ui->lePathToSave_Lpt->setEnabled(false);
    ui->pbBrowse_Lpt->setEnabled(false);
    ui->pbRun_Lpt->setEnabled(false);
    ui->pbCancel_Lpt->setEnabled(true);
    ui->cbVCDsave_Lpt->setEnabled(false);
    ui->cbOSDsave_Lpt->setEnabled(false);
    ui->cbMicronasMode_LPT->setEnabled(false);
    progtimer->start();
}

void MainWindow::normal_mode_Lpt()
{
    ui->cbPortName_Lpt->setEnabled(true);
    ui->cbBufferSize_Lpt->setEnabled(true);
    ui->lePathToSave_Lpt->setEnabled(true);
    ui->pbBrowse_Lpt->setEnabled(true);
    ui->pbRun_Lpt->setEnabled(true);
    ui->pbCancel_Lpt->setEnabled(false);
    ui->cbVCDsave_Lpt->setEnabled(true);
    ui->cbOSDsave_Lpt->setEnabled(true);
    ui->cbMicronasMode_LPT->setEnabled(true);
    progtimer->stop();
}

void MainWindow::disabled_mode_Lpt()
{
    record_mode_Lpt();
    ui->pbCancel_Lpt->setEnabled(false);
}

void MainWindow::on_cbMicronasMode_LPT_stateChanged(int arg1)
{
    lptrec->setMode(ui->cbMicronasMode_LPT->isChecked());
}

void MainWindow::on_pbRun_Lpt_clicked()
{
    lptrec->setPortNumber(ui->cbPortName_Lpt->currentIndex()+1);
    unsigned int bs = 1024;

    switch(ui->cbBufferSize_Lpt->currentIndex())
    {
        case 0:
            bs = 0xFFFF;
            break;
        case 1:
            bs = 4*0xFFFF;
            break;
        case 2:
            bs = 16*0xFFFF;
            break;
        case 3:
            bs =4*16*0xFFFF;
            break;
        case 4:
            bs =16*16*0xFFFF;
            break;
        case 5:
            bs =32*16*0xFFFF;
            break;
        case 6:
            bs =64*16*0xFFFF;
            break;
        default:
        Q_ASSERT(false);
    };

    lptrec->setSampleCount(bs);
    lptrec->startRecord();
}

void MainWindow::on_pbCancel_Lpt_clicked()
{
    lptrec->cancel();
}

void MainWindow::on_pbBrowse_Ftdi_clicked()
{
    QString fn = QFileDialog::getSaveFileName(this,"Data file",ui->lePathToSave_Ftdi->text(),"Data files (*.dat)");
    if (fn=="")return;
    if(fn.endsWith(".dat")==false)fn.append(".dat");
    ui->lePathToSave_Ftdi->setText(fn);
    QSettings settings;
    settings.setValue("LaseFileName",fn);

}
