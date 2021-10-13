#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QPushButton>

namespace Ui {
class IconButton;
}
class IconButton : public QPushButton
{
public:
    int xID,yID; //按钮坐标
    IconButton(QWidget *parent = Q_NULLPTR):QPushButton(parent)
    {
        xID = -1;
        yID = -1;
    };


public: signals:
    void Actived();
};

#endif // ICONBUTTON_H
