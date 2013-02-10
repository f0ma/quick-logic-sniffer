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
        update_lptrec_state();

    }

    if (ui->rbFtdiMode->isChecked())
    {
        ftdirec = new FTDIRecorder(this) ;
        ui->swPages->setCurrentIndex(1);
        connect(ftdirec,SIGNAL(recordStarted()),this,SLOT(ftdi_started()));
        connect(ftdirec,SIGNAL(recordEnded()),this,SLOT(ftdi_finished()));
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

void MainWindow::ftdi_progress(int i)
{
    ui->progressBar_Ftdi->setValue(i);
}

void MainWindow::lpt_progress(int i)
{
    ui->progressBar_Lpt->setValue(i);
}

void MainWindow::ftdi_finished()
{
    normal_mode_Ftdi();
    ui->progressBar_Ftdi->setValue(100);
}

void MainWindow::lpt_finished()
{
   normal_mode_Lpt();
   ui->progressBar_Lpt->setValue(0);
}

void MainWindow::ftdi_started()
{
    record_mode_Ftdi();
    ui->progressBar_Ftdi->setValue(0);
}

void MainWindow::lpt_started()
{
    record_mode_Lpt();
    ui->progressBar_Lpt->setValue(0);
}

void MainWindow::on_pbRun_Ftdi_clicked()
{
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

    switch(ui->cbBufferSize_Ftdi->currentText().toInt())
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
}

void MainWindow::normal_mode_Ftdi()
{
    ui->cbEndpointList_Ftdi->setEnabled(true);
    ui->cbBufferSize_Ftdi->setEnabled(true);
    ui->lePathToSave_Ftdi->setEnabled(true);
    ui->pbBrowse_Ftdi->setEnabled(true);
    ui->pbRun_Ftdi->setEnabled(true);
    ui->pbCancel_Ftdi->setEnabled(false);
    ui->cbVCDsave_Ftdi->setEnabled(true);
    ui->cbOSDsave_Ftdi->setEnabled(true);
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

    switch(ui->cbBufferSize_Lpt->currentText().toInt())
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
}
