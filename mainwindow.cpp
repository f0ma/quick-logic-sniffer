#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->swPages->setCurrentIndex(0);
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
    lptrec->LoadLibrary();
}

void MainWindow::update_ftdirec_state()
{
    ftdirec->LoadLibrary();
    ftdirec->getDevicesList();
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
   ui->progressBar_Lpt->setValue(i);
}

void MainWindow::ftdi_started()
{
    ui->progressBar_Ftdi->setValue(0);
}

void MainWindow::lpt_started()
{
    ui->progressBar_Lpt->setValue(i);
}

void MainWindow::on_bpRun_Ftdi_clicked()
{
    ftdirec->setEnabledDevices();
    ftdirec->setSpeed();
    ftdirec->setsetBufferSize();

}

void MainWindow::on_bpCancel_Ftdi_clicked()
{

}

void MainWindow::on_bpRun_Lpt_clicked()
{
    lptrec->setPort();
    lptrec->setBufferSize();
    lptrec->start();
}

void MainWindow::on_bpCancel_Lpt_clicked()
{
    lptrec->stop();
    lptrec->wait();
}
