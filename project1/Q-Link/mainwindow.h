#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// 对相应函数的注释，见相应的源文件!
//
//

#include <QMainWindow>
#include "game.h"
#include <QPushButton>
#include <QPoint>
#include <QWidget>
#include <QTimer>
#include <QObject>
#include <QKeyEvent>
#include <QPainter>
#include "player.h"
#include "iconbutton.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int Model; //记录游戏的模式
    Game *game;
    MainWindow(int model,QWidget *parent = nullptr);
    ~MainWindow();
    virtual bool eventFilter(QObject *watched, QEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);

private:
    Ui::MainWindow *ui;
    IconButton myButton[12][12]; //地图上按钮
    IconButton *preIcon,*curIcon,*pressedIcon,*PreIcon1,*CurIcon1,*PreIcon2,*CurIcon2; //单人模式下两次激活的按钮，Flash激活时被点击的按钮，双人模式下玩家1和玩家2分别激活的两个按钮
    QTimer *gameTimer,*flashTimer; //记录游戏时间和Flash道具时间
    Player *player,*player1,*player2; //单人模式下玩家，双人模式下玩家1，玩家2
    bool flash,pause; //判断Flash时间是否结束，判断是否暂停

    void beginGame();
    void continueGame();
    void Move(int dx,int dy);
    void Move2(int dx,int dy,int playertype);
    void IconButtonActived(IconButton *Button,int type = 0);
    void Shuffle();
    void Flash();
    void initializeTest();

public slots:
    void handleLink();
    void handleLink1();
    void handleLink2();
    void gameTimerEvent();
    void flashTimerEvent();
    void iconPressed();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_2_pressed();
};
#endif // MAINWINDOW_H
