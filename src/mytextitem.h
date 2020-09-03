#ifndef MYTEXTITEM_H
#define MYTEXTITEM_H

#include <QFont>
#include <QGraphicsTextItem>
#include <QInputDialog>
#include <QWidget>
#include <QDebug>

#include "libhocr/hocr_node.h"
#include "config.h"
#include "mysingletextdialog.h"

using namespace hocr;

class MyTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    //加这个参数呢，是因为总觉得判断if(puncNode) 不太好，干脆来个bool值吧，这样明确一些
    bool hasPuncItem;
    HocrNode * puncNode;

    MyTextItem();
    void setItemSize(int size);
    //    QRectF boundingRect() const;
    MySingleTextDialog* mytextDialog;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    //设置该item的node set
    void setItemNode(HocrNode *value);
    HocrNode *getItemNode() const;
    //每一个item 付一个下面这个结构，代表它的字，和字伴随的图片
    CharAndCharRegionList *charAndPicLists;
    void setCharAndPicList(CharAndCharRegionList *value);
    int x;
    int y;
    int w;
    int h;
    //来判断这个字是哪个模块
    int boardType;
    int nodemode;
    void setBoardType(int value);

    void setPuncNode(HocrNode *value);

    void setNodemode(int value);

private:
    //伴随item的node
    HocrNode* itemNode;

public slots:
    //inputdialog 那里改字的
    void changeTextToWhatYourNodeSet(QString text);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
signals:
    void textSceneAddItemSlot();
};

#endif // MYTEXTITEM_H
