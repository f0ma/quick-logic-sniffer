#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <lptrecorder.h>
#include <ftdirecorder.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    LptRecorder * lptrec;
    FTDIRecorder * ftdirec;
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pbStartNext_clicked();

    void update_lptrec_state();
    void update_ftdirec_state();




    void on_pbBrowse_Lpt_clicked();

    void record_mode_Lpt();
    void normal_mode_Lpt();
    void disabled_mode_Lpt();

    void on_cbMicronasMode_LPT_stateChanged(int arg1);

    void on_pbRun_Lpt_clicked();

    void on_pbCancel_Lpt_clicked();

    void on_pbCancel_Ftdi_clicked();

    void on_pbRun_Ftdi_clicked();

public slots:
    void ftdi_progress(int i);
    void lpt_progress(int i);

    void ftdi_finished();
    void lpt_finished();

    void ftdi_started();
    void lpt_started();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
