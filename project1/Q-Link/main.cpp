#include "mainwindow.h"
#include <QApplication>
#include "dialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog dia;
    if(dia.exec() == QDialog::Accepted){
        MainWindow w(dia.model);
        w.show();
        return a.exec();
    }
    else
      return 0;
}
