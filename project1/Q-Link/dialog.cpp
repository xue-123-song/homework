#include "dialog.h"
#include "ui_dialog.h"


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
   ui->setupUi(this);
   this->setWindowTitle("薛松涛连连看");
}



Dialog::~Dialog()
{
    delete ui;

}

void Dialog::on_pushButton_clicked() //判断游戏模式
{
    QMessageBox::information(NULL, "薛松涛连连看", "欢迎来玩游戏！", QMessageBox::Yes);
    accept();
    model = 1;
    this->close();
}

void Dialog::on_pushButton_2_clicked()
{
    reject();
}


void Dialog::on_pushButton_3_clicked()
{
    QMessageBox::information(NULL, "薛松涛连连看", "欢迎来玩游戏！", QMessageBox::Yes);
    accept();
    model = 2;
    this->close();
}

void Dialog::on_pushButton_5_clicked()
{
    QMessageBox::information(NULL, "薛松涛连连看", "欢迎来玩游戏！", QMessageBox::Yes);
    accept();
    model = 3;
    this->close();
}

void Dialog::on_pushButton_4_clicked()
{
    QMessageBox::information(NULL, "薛松涛连连看", "欢迎来玩游戏！", QMessageBox::Yes);
    accept();
    model = 4;
    this->close();
}
