#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("2micom.ru");
    QCoreApplication::setOrganizationDomain("2micom.ru");
    QCoreApplication::setApplicationName("quick-logic-sniffer");
    MainWindow w;
    w.show();
    
    return a.exec();
}
