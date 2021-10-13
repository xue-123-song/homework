#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QAction>
#include <QMessageBox>
#include <QPainter>
#include <QLine>
#include <iostream>
#include <QFile>
#include <QString>

const int IconSize = 70; //图片大小
const int totalTime =  5 * 60 * 1000; // 总时间
const int intervalTime = 500; //timebar间隔时间
const QString iconReleasedStyle = "";//方块消除后取消激发态
const QString iconActivedStyle = "background-color: rgb(255, 255, 12);"; //激发态
const QString iconHintStyle = "background-color: rgb(255, 0, 0);";

MainWindow::MainWindow(int model,QWidget *parent) //构造函数，完成初始化，并且根据游戏类型，调用相应初始化地图函数
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Model = model; //确定模式
    flash = false;
    pause = false;
    curIcon = new IconButton(this);
    preIcon = new IconButton(this);
    pressedIcon = new IconButton(this);
    CurIcon1 = new IconButton(this);
    PreIcon1 = new IconButton(this);
    CurIcon2 = new IconButton(this);
    PreIcon2 = new IconButton(this);
    player = new Player(this);
    player1 = new Player(this);
    player2 = new Player(this);
    player1->x = 0;
    player1->y = 0;
    player2->x = 11;
    player2->y = 11;
    curIcon = NULL;
    preIcon = NULL;
    pressedIcon = NULL;
    CurIcon1 = NULL;
    PreIcon1 = NULL;
    CurIcon2 = NULL;
    PreIcon2 = NULL;
    ui->setupUi(this);
    this->setWindowTitle("薛松涛连连看~");
    ui->centralwidget->installEventFilter(this);
    ui->timeBar->setMaximum(totalTime);
    ui->timeBar->setMinimum(0);
    ui->timeBar->setValue(totalTime);
    gameTimer = new QTimer(this);
    flashTimer = new QTimer(this);
    connect(gameTimer, SIGNAL(timeout()), this, SLOT(gameTimerEvent())); //实现时间减少显示
    gameTimer->start(intervalTime);
    if(Model == 1 || Model == 3)
        beginGame();
    if(Model == 2 || Model == 4)
        continueGame();
    if(Model == 5)
        initializeTest();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(game) delete game;
}

void MainWindow::beginGame() //新游戏构建地图
{
    game = new Game;
    QPixmap iconPix;
    QString s;
    if(Model == 1){
        game->iniGameMap();
        s = "Scores: " + QString::number(player->mark);
        ui->textEdit->setText(s); //显示得分
        player->playerButton.setParent(this);
        player->playerButton.setGeometry(100+(player->x)*IconSize,50+(player->y)*IconSize,IconSize,IconSize);
        player->playerButton.show();
        iconPix.load("://res/image/1.png");
        QIcon Picon(iconPix);
        player->playerButton.setIcon(Picon);
        player->playerButton.setIconSize(QSize(IconSize, IconSize));
    }
    if(Model == 3){
        game->iniGamMap2();
        s = "(1)Scores: " + QString::number(player1->mark) + "\n" +"(2)Scores: " + QString::number(player2->mark);
        ui->textEdit->setText(s); //显示得分
        player1->playerButton.setParent(this);
        player1->playerButton.setGeometry(100+(player1->x)*IconSize,50+(player1->y)*IconSize,IconSize,IconSize);
        player1->playerButton.show();
        iconPix.load("://res/image/1.png");
        QIcon Picon(iconPix);
        player1->playerButton.setIcon(Picon);
        player1->playerButton.setIconSize(QSize(IconSize, IconSize));

        player2->playerButton.setParent(this);
        player2->playerButton.setGeometry(100+(player2->x)*IconSize,50+(player2->y)*IconSize,IconSize,IconSize);
        player2->playerButton.show();
        iconPix.load("://res/image/12.png");
        QIcon Picon2(iconPix);
        player2->playerButton.setIcon(Picon2);
        player2->playerButton.setIconSize(QSize(IconSize, IconSize));
    }
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 12; j++){
            myButton[i][j].setParent(this);
            myButton[i][j].setGeometry(100+j*IconSize,50+i*IconSize,IconSize,IconSize);
            myButton[i][j].xID = j;
            myButton[i][j].yID = i;
            myButton[i][j].show();
            if(game->gameMap[i][j] != 0){
                QString fileString;
                fileString.sprintf(":/res/iimage/%d.png", game->gameMap[i][j]);
                iconPix.load(fileString);
                QIcon icon(iconPix);
                myButton[i][j].setIcon(icon);
                myButton[i][j].setIconSize(QSize(IconSize, IconSize));
            }
            else{
                myButton[i][j].setIcon(QIcon(""));
                myButton[i][j].setFlat(true);
            }
         connect(&myButton[i][j], SIGNAL(pressed()), this, SLOT(iconPressed()));
        }
    }
}

void MainWindow::continueGame() //读取存档构建地图
{
    int leftTime = 0;
    game = new Game;
    QPixmap iconPix;
    QString s;
    ui->textEdit->setText(s);
    if(Model == 2){
        leftTime = game->continueGameMap(player);
        ui->timeBar->setValue(leftTime);
        s = "Scores: " + QString::number(player->mark);
        ui->textEdit->setText(s);
        player->playerButton.setParent(this);
        player->playerButton.setGeometry(100+(player->x)*IconSize,50+(player->y)*IconSize,IconSize,IconSize);
        player->playerButton.show();
        iconPix.load("://res/image/1.png");
        QIcon Picon(iconPix);
        player->playerButton.setIcon(Picon);
        player->playerButton.setIconSize(QSize(IconSize, IconSize));
    }
    if(Model == 4){
        leftTime = game->continueGameMap2(player1,player2);
        ui->timeBar->setValue(leftTime);
        s = "(1)Scores: " + QString::number(player1->mark) + "\n" +"(2)Scores: " + QString::number(player2->mark);
        ui->textEdit->setText(s);
        player1->playerButton.setParent(this);
        player1->playerButton.setGeometry(100+(player1->x)*IconSize,50+(player1->y)*IconSize,IconSize,IconSize);
        player1->playerButton.show();
        iconPix.load("://res/image/1.png");
        QIcon Picon(iconPix);
        player1->playerButton.setIcon(Picon);
        player1->playerButton.setIconSize(QSize(IconSize, IconSize));

        player2->playerButton.setParent(this);
        player2->playerButton.setGeometry(100+(player2->x)*IconSize,50+(player2->y)*IconSize,IconSize,IconSize);
        player2->playerButton.show();
        iconPix.load("://res/image/12.png");
        QIcon Picon2(iconPix);
        player2->playerButton.setIcon(Picon2);
        player2->playerButton.setIconSize(QSize(IconSize, IconSize));
    }
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 12; j++){
            myButton[i][j].setParent(this);
            myButton[i][j].setGeometry(100+j*IconSize,50+i*IconSize,IconSize,IconSize);
            myButton[i][j].xID = j;
            myButton[i][j].yID = i;
            myButton[i][j].show();
            if(game->gameMap[i][j] != 0){
                QString fileString;
                fileString.sprintf(":/res/iimage/%d.png", game->gameMap[i][j]);
                iconPix.load(fileString);
                QIcon icon(iconPix);
                myButton[i][j].setIcon(icon);
                myButton[i][j].setIconSize(QSize(IconSize, IconSize));
            }
            else{
                myButton[i][j].setIcon(QIcon(""));
                myButton[i][j].setFlat(true);
            }
         connect(&myButton[i][j], SIGNAL(pressed()), this, SLOT(iconPressed()));
        }
    }
}

void MainWindow::gameTimerEvent() //显示剩余游戏时间
{
    if(ui->timeBar->value() == 0){
        gameTimer->stop();
        QMessageBox::information(this, "time out", "come on!");
        this->close();
    }
    else{
        ui->timeBar->setValue(ui->timeBar->value() - intervalTime);
    }

}

void MainWindow::IconButtonActived(IconButton *Button,int type) //Button激活后进行操作函数，type = 0,1,2 分别表示单人模式，双人模式下玩家1，玩家2
{
    if(type == 0){
        curIcon = Button;
        if(!preIcon){
            curIcon->setStyleSheet(iconActivedStyle);
            preIcon = curIcon;
        }
        else{
            if(curIcon != preIcon){
                curIcon->setStyleSheet(iconActivedStyle);
                if(game->linkTwoButton(preIcon->xID, preIcon->yID, curIcon->xID, curIcon->yID)){
                    update();
                    QTimer::singleShot(200, this, SLOT(handleLink()));
                    player->mark++;
                    QString s;
                    s = "Scores: " + QString::number(player->mark);
                    ui->textEdit->setText(s);
                    if (game->isWin()){  //是否获胜
                        QMessageBox::information(this, "over", "good");
                        this->close();
                        return;
                        }
                    if(game->isCanSolved() == false){    //消去后判断是否还有解
                        QMessageBox::information(this, "no answer","it is over");
                        this->close();
                        return;
                        }
                }
                else{
                    preIcon->setStyleSheet(iconReleasedStyle);
                    curIcon->setStyleSheet(iconReleasedStyle);
                    preIcon = NULL;
                    curIcon = NULL;
                }
            }
            else if(curIcon == preIcon){
                preIcon->setStyleSheet(iconReleasedStyle);
                curIcon->setStyleSheet(iconReleasedStyle);
                preIcon = NULL;
                curIcon = NULL;
            }
        }
    }

    if(type == 1){
        CurIcon1 = Button;
        if(!PreIcon1){
            CurIcon1->setStyleSheet(iconActivedStyle);
            PreIcon1 = CurIcon1;
        }
        else{
            if(CurIcon1 != PreIcon1){
                CurIcon1->setStyleSheet(iconActivedStyle);
                if(game->linkTwoButton(PreIcon1->xID, PreIcon1->yID, CurIcon1->xID, CurIcon1->yID)){
                    update();
                    QTimer::singleShot(100, this, SLOT(handleLink1()));
                    player1->mark++;
                    QString s;
                    s = "(1)Scores: " + QString::number(player1->mark) + "\n" +"(2)Scores: " + QString::number(player2->mark);
                    ui->textEdit->setText(s);
                    if (game->isWin()){  //是否获胜
                        QMessageBox::information(this, "over", "good");
                        this->close();
                        return;
                        }
                    if(game->isCanSolved() == false){    //消去后判断是否还有解
                        QMessageBox::information(this, "no answer","it is over");
                        this->close();
                        return;
                        }
                }
                else{
                    PreIcon1->setStyleSheet(iconReleasedStyle);
                    CurIcon1->setStyleSheet(iconReleasedStyle);
                    PreIcon1 = NULL;
                    CurIcon1 = NULL;
                }
            }
            else if(CurIcon1 == PreIcon1){
                PreIcon1->setStyleSheet(iconReleasedStyle);
                CurIcon1->setStyleSheet(iconReleasedStyle);
                PreIcon1 = NULL;
                CurIcon1 = NULL;
            }
        }
    }

    if(type == 2){
        CurIcon2 = Button;
        if(!PreIcon2){
            CurIcon2->setStyleSheet(iconActivedStyle);
            PreIcon2 = CurIcon2;
        }
        else{
            if(CurIcon2 != PreIcon2){
                CurIcon2->setStyleSheet(iconActivedStyle);
                if(game->linkTwoButton(PreIcon2->xID, PreIcon2->yID, CurIcon2->xID, CurIcon2->yID)){
                    update();
                    QTimer::singleShot(100, this, SLOT(handleLink2()));
                    player2->mark++;
                    QString s;
                    s = "(1)Scores: " + QString::number(player1->mark) + "\n" +"(2)Scores: " + QString::number(player2->mark);
                    ui->textEdit->setText(s);
                    if (game->isWin()){  //是否获胜
                        QMessageBox::information(this, "over", "good");
                        this->close();
                        return;
                        }
                    if(game->isCanSolved() == false){    //消去后判断是否还有解
                        QMessageBox::information(this, "no answer","it is over");
                        this->close();
                        return;
                        }
                }
                else{
                    PreIcon2->setStyleSheet(iconReleasedStyle);
                    CurIcon2->setStyleSheet(iconReleasedStyle);
                    PreIcon2 = NULL;
                    CurIcon2 = NULL;
                }
            }
            else if(CurIcon1 == PreIcon1){
                PreIcon2->setStyleSheet(iconReleasedStyle);
                CurIcon2->setStyleSheet(iconReleasedStyle);
                PreIcon2 = NULL;
                CurIcon2 = NULL;
            }
        }
    }
}

void MainWindow::handleLink() //单人模式下，划线结束后操作，不再显示消去的两个Button
{
    game->paintPoints.clear();
    preIcon->setStyleSheet(iconReleasedStyle);
    curIcon->setStyleSheet(iconReleasedStyle);
    preIcon->setIcon(QIcon(""));
    preIcon->setFlat(true);
    curIcon->setIcon(QIcon(""));
    curIcon->setFlat(true);
    preIcon = NULL;
    curIcon = NULL;
    update();
}

void MainWindow::handleLink1() //双人模式下，玩家1划线结束后操作，不再显示消去的两个Button
{
    game->paintPoints.clear();
    PreIcon1->setStyleSheet(iconReleasedStyle);
    CurIcon1->setStyleSheet(iconReleasedStyle);
    PreIcon1->setIcon(QIcon(""));
    PreIcon1->setFlat(true);
    CurIcon1->setIcon(QIcon(""));
    CurIcon1->setFlat(true);
    PreIcon1 = NULL;
    CurIcon1 = NULL;
    update();
}

void MainWindow::handleLink2() //双人模式下，玩家2划线结束后操作，不再显示消去的两个Button
{
    game->paintPoints.clear();
    PreIcon2->setStyleSheet(iconReleasedStyle);
    CurIcon2->setStyleSheet(iconReleasedStyle);
    PreIcon2->setIcon(QIcon(""));
    PreIcon2->setFlat(true);
    CurIcon2->setIcon(QIcon(""));
    CurIcon2->setFlat(true);
    PreIcon2 = NULL;
    CurIcon2 = NULL;
    update();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) //地图更新时进行划线
{

    if (event->type() == QEvent::Paint){
        QPainter painter(this->ui->centralwidget);
        QPen pen;
        QColor color(rand() % 256, rand() % 256, rand() % 256);
        pen.setColor(color);
        pen.setWidth(5);
        painter.setPen(pen);
        for (int i = 0; i < (int)game->paintPoints.size()- 1; i++){
            PaintPoint p1 = game->paintPoints[i];
            PaintPoint p2 = game->paintPoints[i + 1];
            QPoint pos1(100 + p1.x * IconSize + IconSize / 2,50 + p1.y * IconSize + IconSize / 2);
            QPoint pos2(100 + p2.x * IconSize + IconSize / 2,50 + p2.y * IconSize + IconSize / 2);
            painter.drawLine(pos1, pos2);
        }
        game->paintPoints.clear();
    }
    else
        return QMainWindow::eventFilter(watched, event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) //处理按下相应按键（上下左右）
{
    if(pause == true)
        return;
    if(Model == 1 || Model == 2){
        switch (event->key()) {

        case Qt::Key_W:
            Move(0,-1);
            break;

        case Qt::Key_S:
            Move(0,1);
            break;

        case Qt::Key_A:
            Move(-1,0);
            break;

        case Qt::Key_D:
            Move(1,0);
            break;
        }
    }
    if(Model == 3 || Model == 4){
        switch (event->key()) {

        case Qt::Key_W:
            Move2(0,-1,1);
            break;

        case Qt::Key_S:
            Move2(0,1,1);
            break;

        case Qt::Key_A:
            Move2(-1,0,1);
            break;

        case Qt::Key_D:
            Move2(1,0,1);
            break;

        case Qt::Key_I:
            Move2(0,-1,2);
            break;

        case Qt::Key_K:
            Move2(0,1,2);
            break;

        case Qt::Key_J:
            Move2(-1,0,2);
            break;

        case Qt::Key_L:
            Move2(1,0,2);
            break;
        }
    }
}

void MainWindow::Shuffle() //实现道具Shuffle
{
    preIcon = NULL;
    curIcon = NULL;
    for(int i = 1;i < 11;i++){
        for(int j = 1;j < 11;j++){
            myButton[i][j].setStyleSheet(iconReleasedStyle);
            myButton[i][j].setFlat(false);
        }
    }
    srand((unsigned)time(0));
    for(int i = 1; i < 11; i++){
        for(int j = 1; j < 11; j++){
            int randomx,randomy,t;
            randomx = (rand() + rand()) % 10 + 1;
            randomy = rand() * rand() % 10 + 1;
            t = game->gameMap[i][j];
            game->gameMap[i][j] = game->gameMap[randomy][randomx];
            game->gameMap[randomy][randomx] = t;
        }
    }
    srand((unsigned)time(0));
    player->x = rand() % 12;
    player->y = (rand() * 5 * rand() )% 12;
    if((player->x * player->y) / 2 == 0){
        if((player->x + player->y) / 2 == 0)
            player->x = 0;
        else
            player->x = 11;
    }
    else{
        if((player->x + player->y)/ 2 == 0)
            player->y = 0;
        else
            player->y = 11;
    }
    player->playerButton.setGeometry(100+(player->x)*IconSize,50+(player->y)*IconSize,IconSize,IconSize);
    for(int i = 1; i < 11; i++){
        for(int j = 1; j < 11; j++){
            myButton[i][j].setGeometry(100+j*IconSize,50+i*IconSize,IconSize,IconSize);
            myButton[i][j].xID = j;
            myButton[i][j].yID = i;
            myButton[i][j].show();
            if(game->gameMap[i][j] != 0){
                QPixmap iconPix;
                QString fileString;
                fileString.sprintf(":/res/iimage/%d.png", game->gameMap[i][j]);
                iconPix.load(fileString);
                QIcon icon(iconPix);
                myButton[i][j].setIcon(icon);
                myButton[i][j].setIconSize(QSize(IconSize, IconSize));
            }
            else{
                myButton[i][j].setIcon(QIcon(""));
                myButton[i][j].setFlat(true);
            }

        }
    }
    update();
}

void MainWindow::iconPressed() //Flash激活时，点击某一Button后，进行操作
{
    if(flash == false)
        return;
    if(pause == true)
        return;
    pressedIcon = dynamic_cast<IconButton *>(sender());
    int px,py;
    px = pressedIcon->xID;
    py = pressedIcon->yID;
    if(game->isCango(player->x,player->y,px,py) != true)
        return;

    if(game->gameMap[py][px] == 0){
        player->x = px;
        player->y = py;
        player->playerButton.setGeometry(100+(player->x)*IconSize,50+(player->y)*IconSize,IconSize,IconSize);
        update();
        return;
    }

    if(game->gameMap[py][px] == 101){
        int addtime;
        addtime = ui->timeBar->value() + 10000;
        if(addtime > totalTime)
            addtime = totalTime;
        ui->timeBar->setValue(addtime);
        game->gameMap[py][px] = 0;
        myButton[py][px].setIcon(QIcon(""));
        myButton[py][px].setFlat(true);
        int dx,dy;
        if(game->gameMap[py-1][px] == 0){
            dx = px;
            dy = py - 1;
        }
        if(game->gameMap[py+1][px] == 0){
            dx = px;
            dy = py + 1;
        }
        if(game->gameMap[py][px-1] == 0){
            dx = px - 1;
            dy = py;
        }
        if(game->gameMap[py][px+1] == 0){
            dx = px + 1;
            dy = py;
        }
        player->x = dx;
        player->y = dy;
        player->playerButton.setGeometry(100+(player->x)*IconSize,50+(player->y)*IconSize,IconSize,IconSize);
        update();
        return;
    }

    if(game->gameMap[py][px] == 200){
        game->gameMap[py][px] = 0;
        Shuffle();
        return;
    }

    IconButtonActived(&myButton[py][px]);
    int dx,dy;
    if(game->gameMap[py-1][px] == 0){
        dx = px;
        dy = py - 1;
    }
    if(game->gameMap[py+1][px] == 0){
        dx = px;
        dy = py + 1;
    }
    if(game->gameMap[py][px-1] == 0){
        dx = px - 1;
        dy = py;
    }
    if(game->gameMap[py][px+1] == 0){
        dx = px + 1;
        dy = py;
    }
    player->x = dx;
    player->y = dy;
    player->playerButton.setGeometry(100+(player->x)*IconSize,50+(player->y)*IconSize,IconSize,IconSize);
    update();
    return;
}

void MainWindow::flashTimerEvent() //flash时间结束后，道具效果消失
{
    flash = false;
}

void MainWindow::Flash() //激活道具Flash
{
    flash = true;
    connect(flashTimer, SIGNAL(timeout()), this, SLOT(flashTimerEvent()));
    flashTimer->start(5000);
}

void MainWindow::Move(int dx, int dy) //单人模式下，处理角色移动，参数表示x,y方向上位移
{
    int nx,ny;
    if(pause == true)
        return;
    nx = player->x + dx;
    ny = player->y + dy;
    if(nx < 0 || nx > 11)
        return;
    if(ny < 0 || ny > 11)
        return;
    if(game->gameMap[ny][nx] == 0){
        player->x = nx;
        player->y = ny;
        player->playerButton.setGeometry(100+(player->x)*IconSize,50+(player->y)*IconSize,IconSize,IconSize);
        update();
    }
    else{
        if(game->gameMap[ny][nx] == 101){ //addtime道具
            int addtime;
            addtime = ui->timeBar->value() + 10000;
            if(addtime > totalTime)
                addtime = totalTime;
            ui->timeBar->setValue(addtime);
            game->gameMap[ny][nx] = 0;
            myButton[ny][nx].setIcon(QIcon(""));
            myButton[ny][nx].setFlat(true);
            return;
        }
        if(game->gameMap[ny][nx] == 200){ //Shuffle道具
            game->gameMap[ny][nx] = 0;
            myButton[ny][nx].setIcon(QIcon(""));
            myButton[ny][nx].setFlat(true);
            Shuffle();
            if(game->isCanSolved() == false)
                 QMessageBox::information(this, "no answer", "it is over");
            return;
        }
         if(game->gameMap[ny][nx] == 300){ //Flash道具
             game->gameMap[ny][nx] = 0;
             myButton[ny][nx].setIcon(QIcon(""));
             myButton[ny][nx].setFlat(true);
             Flash();
             return;
         }
        IconButtonActived(&myButton[ny][nx]); //一般按钮
        return;
    }
    return;
}

void MainWindow::Move2(int dx, int dy, int playertype) //双人模式下，处理角色移动，参数分别表示x,y方向上位移，playertype以区分玩家1和玩家2
{
    int nx,ny;
    if(pause == true)
        return;
    if(playertype == 1){
        nx = player1->x + dx;
        ny = player1->y + dy;
        if(nx < 0 || nx > 11)
            return;
        if(ny < 0 || ny > 11)
            return;
        if(game->gameMap[ny][nx] == 0){
            player1->x = nx;
            player1->y = ny;
            player1->playerButton.setGeometry(100+(player1->x)*IconSize,50+(player1->y)*IconSize,IconSize,IconSize);
            update();
        }
        else{
            IconButtonActived(&myButton[ny][nx],1);
            return;
        }
    }
   if(playertype == 2){
       nx = player2->x + dx;
       ny = player2->y + dy;
       if(nx < 0 || nx > 11)
           return;
       if(ny < 0 || ny > 11)
           return;
       if(game->gameMap[ny][nx] == 0){
           player2->x = nx;
           player2->y = ny;
           player2->playerButton.setGeometry(100+(player2->x)*IconSize,50+(player2->y)*IconSize,IconSize,IconSize);
           update();
       }
       else{
           IconButtonActived(&myButton[ny][nx],2);
           return;
       }
   }
    return;
}


void MainWindow::initializeTest() //测试初始化地图
{
    game = new Game;
    for(int i = 0 ;i < 12;i++){
        for(int j = 0;j < 12;j++){
            game->gameMap[i][j] = 0;
        }
    }
    game->gameMap[1][1] = 1;
    game->gameMap[1][4] = 1;
    game->gameMap[2][1] = 2;
    game->gameMap[2][2] = 2;
    game->gameMap[3][4] = 2;
    game->gameMap[4][1] = 1;
    game->gameMap[4][2] = 1;
    game->gameMap[4][3] = 2;
    game->gameMap[5][1] = 101;
}

void MainWindow::on_pushButton_clicked() //实现暂停
{
     pause = !pause;
     if(pause == true){
         ui->pushButton->setText("继续");
         gameTimer->stop();
     }
     else{
         ui->pushButton->setText("暂停");
         gameTimer->start(intervalTime);
     }
}

void MainWindow::on_pushButton_2_clicked() //无作用槽函数，UI界面误点击生成
{
    return;
}

void MainWindow::on_pushButton_2_pressed() //实现缓存，单人模式数据存入data，双人模式数据存入data2
{
    if(Model == 1 || Model == 2){
        QFile file("C:\\Users\\49103\\Desktop\\Q-Link\\res\\data.txt",this);
        QString str = {""},s;
        file.open(QIODevice::WriteOnly);
        for(int i =0;i < 12;i++){
            for(int j = 0;j < 12;j++){
                  s = QString::number(game->gameMap[i][j]);
                  str = str + s +" ";
            }
            str = str + "\n";
         file.write(str.toUtf8());
         str = "";
        }
        str = "";
        s = QString::number(player->x);
        str = str + s + " ";
        s = QString::number(player->y);
        str = str + s + " ";
        s = QString::number(player->mark);
        str = str + s + "\n";
        file.write(str.toUtf8());
        str = QString::number(ui->timeBar->value());
        file.write(str.toUtf8());
        file.close();
    }
    if(Model == 3 || Model == 4){
        QFile file("C:\\Users\\49103\\Desktop\\Q-Link\\res\\data2.txt",this);
        QString str = {""},s;
        file.open(QIODevice::WriteOnly);
        for(int i =0;i < 12;i++){
            for(int j = 0;j < 12;j++){
                  s = QString::number(game->gameMap[i][j]);
                  str = str + s +" ";
            }
            str = str + "\n";
         file.write(str.toUtf8());
         str = "";
        }
        str = "";
        s = QString::number(player1->x);
        str = str + s + " ";
        s = QString::number(player1->y);
        str = str + s + " ";
        s = QString::number(player2->x);
        str = str + s + " ";
        s = QString::number(player2->y);
        str = str + s + " ";
        s = QString::number(player1->mark);
        str = str + s + " ";
        s = QString::number(player2->mark);
        str = str + s + "\n";
        file.write(str.toUtf8());
        str = QString::number(ui->timeBar->value());
        file.write(str.toUtf8());
        file.close();
    }
}
