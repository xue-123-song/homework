#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include <QPixmap>
#include <QPalette>
#include <QMessageBox>


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0); //与玩家对话框，选择游戏模式，是否退出
    ~Dialog();
    int model;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Dialog *ui;

};

#endif // DIALOG_H
