#include "player.h"


Player::Player(QObject *parent) : QObject(parent),mark(0) //初始时，随机生成人物位置
{
     srand((unsigned)time(0));
     x = rand() % 12;
     y = (rand() * 5 * rand() )% 12;
     if((x * y) / 2 == 0){
         if((x + y) / 2 == 0)
             x = 0;
         else
             x = 11;
     }
     else{
         if((x + y)/ 2 == 0)
             y = 0;
         else
             y = 11;
     }
}


