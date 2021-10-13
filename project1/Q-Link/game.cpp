#include "game.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <QFile>
#include "player.h"

const int MAX_ICON = 15;

Game::Game()
{
}

bool Game::isWin()    //是否获胜
{
    for(int i = 1; i < 11; i++){
        for(int j = 1; j < 11; j++){
           if(gameMap[i][j] != 0 && gameMap[i][j] != 101 && gameMap[i][j] != 200 && gameMap[i][j] != 300)
               return false;
        }
    }
    return true;
}

void Game::iniGameMap() //单人模式新建地图
{
    int iconID = 0;
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 12; j++){
            gameMap[i][j] = 0;
        }
    }
    for(int i = 1; i < 8; i++){
        for(int j = 1; j < 10; j++){
            gameMap[i][j] = iconID % MAX_ICON + 1;
            gameMap[i][j+1] = iconID % MAX_ICON + 1;
            iconID++;
        }
    }
    gameMap[1][1] = 101;// + 1s
    gameMap[2][2] = 200;//shuffle
    gameMap[3][3] = 300;//flash
    srand((unsigned)time(0));
    for(int i = 1; i < 11; i++){
        for(int j = 1; j < 11; j++){
            int randomx,randomy,t;
            randomx = (rand() + rand()) % 10 + 1;
            randomy = rand() * rand() % 10 + 1;
            t = gameMap[i][j];
            gameMap[i][j] = gameMap[randomy][randomx];
            gameMap[randomy][randomx] = t;
        }
    }
    paintPoints.clear();
}

void Game::iniGamMap2() //双人模式新建地图
{
    int iconID = 0;
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 12; j++){
            gameMap[i][j] = 0;
        }
    }
    for(int i = 1; i < 8; i++){
        for(int j = 1; j < 10; j++){
            gameMap[i][j] = iconID % MAX_ICON + 1;
            gameMap[i][j+1] = iconID % MAX_ICON + 1;
            iconID++;
        }
    }
    srand((unsigned)time(0));
    for(int i = 1; i < 11; i++){
        for(int j = 1; j < 11; j++){
            int randomx,randomy,t;
            randomx = (rand() + rand()) % 10 + 1;
            randomy = rand() * rand() % 10 + 1;
            t = gameMap[i][j];
            gameMap[i][j] = gameMap[randomy][randomx];
            gameMap[randomy][randomx] = t;
        }
    }
    paintPoints.clear();
}

bool Game::isCanSolved() //判断是否还有解，有解返回true
{
    int tx,ty,txFlash,tyFlash;
    txFlash = tyFlash = tx = ty = 0;
    for(int i = 1;i < 11;i++){
        for(int j = 1;j < 11;j++){
            if(gameMap[i][j] == 200){
                if(isCango(0,0,j,i)){
                    if(tx != 0 && ty != 0)
                        gameMap[ty][tx] =101;
                    if(txFlash != 0 && tyFlash != 0)
                        gameMap[tyFlash][txFlash] = 300;
                    return true;
                }
            }
            if(gameMap[i][j] == 101){
                gameMap[i][j] = 0;
                tx = j;
                ty = i;
            }
            if(gameMap[i][j] == 300){
                gameMap[i][j] = 0;
                txFlash = j;
                tyFlash = i;
            }
        }
    }
    for(int i = 1;i < 11;i++){
        for(int j = 1;j < 11;j++){
            if(gameMap[i][j] == 0 || gameMap[i][j] == 200)
                continue;
            for(int k = i;k < 11;k++){
                for(int l = 1;l < 11;l++){
                    if(k == i && l == j)
                        continue;
                    if(gameMap[k][l] != gameMap[i][j])
                        continue;
                    if(!isCanLinktoJudge(j,i,l,k))
                        continue;
                    if(!isCango(0,0,j,i))
                        continue;
                    if(!isCango(0,0,l,k))
                        continue;
                    if(tx != 0 && ty != 0)
                        gameMap[ty][tx] =101;
                    if(txFlash != 0 && tyFlash != 0)
                        gameMap[tyFlash][txFlash] =300;
                    return true;
                }
            }
        }
    }
    if(tx != 0 && ty != 0)
        gameMap[ty][tx] =101;
    if(txFlash != 0 && tyFlash != 0)
        gameMap[tyFlash][txFlash] = 300;
    return false;
}

void Game::dfstoJudge(int x, int y, int dx, int dy, int count, Direction direction) //判断是否有解时调用的搜索函数，参数分别表示当前坐标，目标坐标，拐角数，上次前进方向
{
    if(flagtoJudge)
        return;
    if(x < 0 || x > 11 || y < 0 || y > 11)
        return;
    if(count >= 3)
        return;
    if(book[y][x] != 0)
        return;
    if(x == dx && y == dy){
        flagtoJudge = true;
        return;
    }
    if(gameMap[y][x] != 0 && direction != Nod)
        return;

    book[y][x] = 1;

    if(direction != Up && direction != Nod){
        dfstoJudge(x,y - 1,dx,dy,count + 1,Up);
        if(flagtoJudge)
            return;
    }
    else{
         dfstoJudge(x,y - 1,dx,dy,count,Up);
        if(flagtoJudge)
            return;
    }

    if(direction != Down && direction != Nod){
        dfstoJudge(x,y + 1,dx,dy,count + 1,Down);
        if(flagtoJudge)
            return;
    }
    else{
        dfstoJudge(x,y + 1,dx,dy,count,Down);
        if(flagtoJudge)
            return;
    }

    if(direction != Right && direction != Nod){
        dfstoJudge(x + 1,y,dx,dy,count + 1,Right);
        if(flagtoJudge)
            return;
    }
    else{
         dfstoJudge(x + 1,y,dx,dy,count,Right);
        if(flagtoJudge)
            return;
    }

    if(direction != Left && direction != Nod){
        dfstoJudge(x - 1,y,dx,dy,count + 1,Left);
        if(flagtoJudge)
            return;
    }
    else{
         dfstoJudge(x - 1,y,dx,dy,count,Left);
        if(flagtoJudge)
            return;
    }
    book[y][x] = 0;
}

bool Game::isCanLinktoJudge(int x1, int y1, int x2, int y2) //判断是否有解时，调用搜索函数前准备工作
{
    flagtoJudge = false;
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 12; j++){
            book[j][i] = 0;
        }
    }
    dfstoJudge(x1,y1,x2,y2,0,Nod);
    return flagtoJudge;
}

void Game::dfs(int x, int y, int dx, int dy,int count, Direction direction) //判断两激活Button是否能相连，参数分别表示当前坐标，目标坐标，拐角数，上次前进方向
{
    if(flag)
        return;
    if(x < 0 || x > 11 || y < 0 || y > 11)
        return;
    if(count >= 3)
        return;
    if(book[y][x] != 0)
        return;
    if(x == dx && y == dy){
        flag = true;
        return;
    }
    if(gameMap[y][x] != 0 && direction != Nod)
        return;

    PaintPoint p(x,y);

    book[y][x] = 1;

    if(direction != Up && direction != Nod){
        paintPoints.push_back(p);
        dfs(x,y - 1,dx,dy,count + 1,Up);
        if(flag)
            return;
        paintPoints.pop_back();
    }
    else{
         dfs(x,y - 1,dx,dy,count,Up);
        if(flag)
            return;
    }

    if(direction != Down && direction != Nod){
        paintPoints.push_back(p);
        dfs(x,y + 1,dx,dy,count + 1,Down);
        if(flag)
            return;
        paintPoints.pop_back();
    }
    else{
        dfs(x,y + 1,dx,dy,count,Down);
        if(flag)
            return;
    }

    if(direction != Right && direction != Nod){
        paintPoints.push_back(p);
        dfs(x + 1,y,dx,dy,count + 1,Right);
        if(flag)
            return;
        paintPoints.pop_back();
    }
    else{
         dfs(x + 1,y,dx,dy,count,Right);
        if(flag)
            return;
    }

    if(direction != Left && direction != Nod){
        paintPoints.push_back(p);
        dfs(x - 1,y,dx,dy,count + 1,Left);
        if(flag)
            return;
        paintPoints.pop_back();
    }
    else{
         dfs(x - 1,y,dx,dy,count,Left);
        if(flag)
            return;
    }
    book[y][x] = 0;
}

bool Game::isCanLink(int x1, int y1, int x2, int y2) //判断两按钮是否能消去，能则返回true,参数分别表示两按钮坐标
{

    flag = false;

    if (gameMap[y1][x1] == 0 || gameMap[y2][x2] == 0)
        return false;

    if (x1 == x2 && y1 == y2)
        return false;

    if(gameMap[y1][x1] != gameMap[y2][x2])
        return false;

    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 12; j++){
            book[j][i] = 0;
        }
    }
    dfs(x1,y1,x2,y2,0,Nod);
    return flag;
}

bool Game::linkTwoButton(int x1, int y1, int x2, int y2) //判断两按钮能否消去的准备和后续工作，参数分别表示两按钮坐标，能消去返回true
{
    PaintPoint p1(x1,y1);
    paintPoints.push_back(p1);
    if(isCanLink(x1, y1, x2, y2)){
        gameMap[y1][x1] = 0;
        gameMap[y2][x2] = 0;
        PaintPoint p2(x2,y2);
        paintPoints.push_back(p2);
        return true;
    }
    paintPoints.pop_back();
    return false;
}

bool Game::dfsFlash(int x, int y, int dx, int dy) //激活Flash道具，判断所点击区域能否到达的搜索函数，参数分别表示当前坐标和目标坐标
{
    if(x < 0 || x > 11 || y < 0 || y > 11)
        return false;
    if(book[y][x] != 0)
        return false;
    if(x == dx && y == dy)
        return true;
    if(gameMap[y][x] != 0)
        return false;
    book[y][x] = 1;
    if(dfsFlash(x,y-1,dx,dy))
        return true;
    if(dfsFlash(x,y+1,dx,dy))
        return true;
    if(dfsFlash(x-1,y,dx,dy))
        return true;
    if(dfsFlash(x+1,y,dx,dy))
        return true;
    book[y][x] = 0;
    return false;
}

bool Game::isCango(int x, int y, int dx, int dy) //判断dx,dy位置能否到达的准备函数
{
    for(int i = 0; i < 12; i++) {
        for(int j = 0; j < 12; j++)  {
            book[j][i] = 0;
        }
    }
    return dfsFlash(x,y,dx,dy);
}

int Game::continueGameMap(Player *player) //单人模式下读取存档构造地图
{
    QFile file("C:\\Users\\49103\\Desktop\\Q-Link\\res\\data.txt");
    QString str = {""};

    file.open(QIODevice::ReadOnly);
    for(int i = 0;i < 12;i++){
        str =  file.readLine();
        QStringList list = str.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        for(int j = 0;j < 12;j++){
            gameMap[i][j] = list[j].toInt();
        }
    }
    str =  file.readLine();
    QStringList list = str.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    player->x = list[0].toInt();
    player->y = list[1].toInt();
    player->mark = list[2].toInt();
    str =  file.readLine();
    int leftTime = str.toInt();
    file.close();
    return leftTime;
}

int Game::continueGameMap2(Player *player1, Player *player2) //双人模式下读取存档，构造地图
{
    QFile file("C:\\Users\\49103\\Desktop\\Q-Link\\res\\data2.txt");
    QString str = {""};
    file.open(QIODevice::ReadOnly);
    for(int i = 0;i < 12;i++){
        str =  file.readLine();
        QStringList list = str.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        for(int j = 0;j < 12;j++){
            gameMap[i][j] = list[j].toInt();
        }
    }
    str =  file.readLine();
    QStringList list = str.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    player1->x = list[0].toInt();
    player1->y = list[1].toInt();
    player2->x = list[2].toInt();
    player2->y = list[3].toInt();
    player1->mark = list[4].toInt();
    player2->mark = list[5].toInt();
    str =  file.readLine();
    int leftTime = str.toInt();
    file.close();
    return leftTime;
}















