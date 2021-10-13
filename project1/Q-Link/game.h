#ifndef GAME_H
#define GAME_H

#include <vector>
#include <QtDebug>
#include <QFile>
#include "player.h"

struct PaintPoint
{
    PaintPoint(int _x, int _y) : x(_x), y (_y) {}
    int x;
    int y;
};

enum Direction { Nod,Up,Down,Right,Left};

class Game
{
public:
    std::vector<PaintPoint> paintPoints; //存储划线所需点的坐标
    int gameMap[12][12]; //存储地图
    Game();
    bool isWin();
    void iniGameMap();
    void iniGamMap2();
    int continueGameMap(Player *player);
    int continueGameMap2(Player *player1,Player *player2);
    bool linkTwoButton(int x1, int y1, int x2,int y2);
    bool isCango(int x,int y,int dx,int dy);
    bool isCanSolved();

private:
    bool flag,flagtoJudge; //分别在两按钮激活后和判断是否有解时记录是否能相连
    int book[12][12] = {0}; //搜索函数中用来标记已探索过的点
    bool isCanLink(int x1, int y1, int x2, int y2);
    void dfs(int x,int y,int dx,int dy,int count,Direction direction);
    bool dfsFlash(int x,int y,int dx,int dy);
    bool isCanLinktoJudge(int x1, int y1, int x2, int y2);
    void dfstoJudge(int x,int y,int dx,int dy,int count,Direction direction);
};

#endif // GAME_H
