#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPushButton>
#include <QPainter>
#include <QImage>
#include <time.h>
#include "QPoint"


class Player : public QObject
{
    Q_OBJECT
public:
    int x,y,mark;
    explicit Player(QObject *parent = nullptr);
    QPushButton playerButton;
signals:

};

#endif // PLAYER_H
