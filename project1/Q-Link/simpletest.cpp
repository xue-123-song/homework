#include "simpletest.h"

SimpleTest::SimpleTest(QObject *parent) : QObject(parent),w(5)
{
}

void SimpleTest::testCase1() //直线连接
{
    QVERIFY(w.game->linkTwoButton(1,1,4,1) == true);
    w.game->gameMap[1][1] = 1;
    w.game->gameMap[1][4] = 1;
}

void SimpleTest::testCase2() //相邻连接
{
     QVERIFY(w.game->linkTwoButton(1,2,2,2) == true);
     w.game->gameMap[2][1] = 2;
     w.game->gameMap[2][2] = 2;
}

void SimpleTest::testCase3() //不同按钮
{
     QVERIFY(w.game->linkTwoButton(1,1,1,2) == false);
     w.game->gameMap[1][1] = 1;
     w.game->gameMap[2][1] = 2;
}

void SimpleTest::testCase4() //一次折线
{
     QVERIFY(w.game->linkTwoButton(2,2,4,3) == true);
     w.game->gameMap[2][2] = 2;
     w.game->gameMap[3][4] = 2;
}

void SimpleTest::testCase5() //两次折线
{
    QVERIFY(w.game->linkTwoButton(1,1,1,4) == true);
    w.game->gameMap[1][1] = 1;
    w.game->gameMap[4][1] = 1;
}

void SimpleTest::testCase6() //三次折线
{
    QVERIFY(w.game->linkTwoButton(1,1,2,4) == false);
    w.game->gameMap[1][1] = 1;
    w.game->gameMap[4][2] = 1;
}

void SimpleTest::testCase7() //空地情况
{
    QVERIFY(w.game->linkTwoButton(1,1,2,1) == false);
    w.game->gameMap[1][1] = 1;
    w.game->gameMap[1][2] = 0;
}

void SimpleTest::testCase8() //道具情况
{
    QVERIFY(w.game->linkTwoButton(1,1,1,5) == false);
     w.game->gameMap[1][1] = 1;
     w.game->gameMap[5][1] = 101;
}

//QTEST_MAIN(SimpleTest)
