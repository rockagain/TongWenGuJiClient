#ifndef MYITEM_H
#define MYITEM_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QCursor>
#include <QAction>
#include <QMenu>
#include <QKeyEvent>
#include <QObject>

#include "libhocr/hocr_types.h"
#include "libhocr/hocr_node.h"
#include "config.h"
//#include "myscene.h"
using namespace hocr;



class MyItem : public QObject,  public QGraphicsRectItem   //这个qobject必须在前面....
{

    Q_OBJECT

public:


    MyItem();

    hocr::bbox *getBbox() const;
    void setBbox(hocr::bbox *value);
    void setItemAxis();
    //设置Item的 nodemode
    void setNodeMode(int value);
    HocrNode *itemNode;
    int nodeMode;
    //boardtype
    int boardtype;

    void setBoardtype(int value);

    void setIsDeleteAble(bool value);
    //设置是否可以删除
    bool isDeleteAble;
    ItemMode* mode;
    void checkPoint();
    void setMode(int nodeType, bool ischange, bool isdrag, bool isSeletOrNot, bool isDeleteAble, int boardType);
    //控制是变形还是拖动的
    bool isItemChangeOrNot;

    void operator =(const MyItem &currentItem);
    int cursorLocation;
    bool checkPointSwitch;

    QPointF start;
    QPointF end;
    QCursor cursor;
    qreal endX;
    qreal endY;
    qreal startX;
    qreal startY;
    //坐标的结构体
    hocr::bbox *bbox;

    bool getIsCheckOkOrNot() const;
    void setIsCheckOkOrNot(bool value);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    int  type() const;



    QMenu *menu;
    QAction *mAction;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void keyPressEvent(QKeyEvent *event);
private:
    //拖动前的坐标点
    QPointF previousPos;
    QPointF currentPos;

    //是否被checkPoint调用，默认是false  ， 只checkPoint一次 ，后面的话除了改动，否则都不需要checkPoint
    //已经设定了getter 和 setter
    bool isCheckOkOrNot;

    void setHoverCursorShapeAndState(QGraphicsSceneHoverEvent *event);
signals:
    void setSceneAndEditBoardOnFire();

};

#endif // MYITEM_H
