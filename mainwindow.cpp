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
    }

    if (ui->rbFtdiMode->isChecked())
    {
        ftdirec = new FTDIRecorder(this) ;
        ui->swPages->setCurrentIndex(1);
    }


}

void MainWindow::update_lptrec_state()
{
    lptrec->loadLibrary();
}

void MainWindow::update_ftdirec_state()
{
//    ftdirec->LoadLibrary();
//    ftdirec->getDevicesList();
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
    ui->progressBar_Ftdi->setValue(100);
}

void MainWindow::lpt_finished()
{
   ui->progressBar_Lpt->setValue(0);
}

void MainWindow::ftdi_started()
{
    ui->progressBar_Ftdi->setValue(0);
}

void MainWindow::lpt_started()
{
    ui->progressBar_Lpt->setValue(0);
}

void MainWindow::on_bpRun_Ftdi_clicked()
{
//    ftdirec->setEnabledDevices();
//    ftdirec->setSpeed();
//    ftdirec->setsetBufferSize();

}

void MainWindow::on_bpCancel_Ftdi_clicked()
{

}

void MainWindow::on_bpRun_Lpt_clicked()
{
    lptrec->setPortNumber(ui->cbPortName_Lpt->currentIndex()+1);
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

    lptrec->setSampleCount(bs);
    lptrec->startRecord();
}

void MainWindow::on_bpCancel_Lpt_clicked()
{
    lptrec->cancel();
}

void MainWindow::on_pbBrowse_Lpt_clicked()
{

}

void MainWindow::record_mode_Lpt()
{

}

void MainWindow::normal_mode_Lpt()
{

}

void MainWindow::disabled_mode_Lpt()
{

}

void MainWindow::on_cbMicronasMode_LPT_stateChanged(int arg1)
{
    lptrec->setMode(ui->cbMicronasMode_LPT->isChecked());
}
