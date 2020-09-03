#ifndef MYTEXTSCENE_H
#define MYTEXTSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QDebug>
#include <QQueue>
#include <QKeyEvent>
#include "mytextitem.h"
#include "myview.h"

class MyTextScene : public QGraphicsScene
{
    Q_OBJECT
public:
    //要注意需要清空
    QVector<HocrNode*> puncNodes;
    MyTextItem* tempTextItem;
    HocrNode* tempPuncNode;
    MyTextScene();
    MyTextItem* textItem;
    void testSceneRectToSeeIfItsRight();
    QVector<MyTextItem*> words;
    void keyPressEvent(QKeyEvent *event);
    void setView(MyView *value);
    MyTextItem * currentTextItem;
    int boardType;
    void setBoardType(int value);
    //下面两个int居然也只和整页校对 走字有关了，  新写的上下走字的聚类校对那里，不用这俩了
    int keyUpInt;
    int keyDownInt;
    //下面这个和聚类校对走字相关，  就是说判断上下那里
    int keyPressdInt;
    QVector<MyTextItem*> myTextItems;

    void setImage(QImage *image);
    //下面函数只和整页校对  走字显示有关
    int proof_CurrentColumnIndex;   //当前在那行
    int proof_wholeColumnNum;     //一共多少行
    int proof_WordNumPerColumn;  //每行多少字
    //把这些东西都显示出来，调用两处，一处是每次敲Item的key，第二次是每次循环
    void showPuncItem();

public slots:
    void checkSceneRect(const QRectF &rect);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
private slots:
    void resetViewsScrollbarvalue(QGraphicsItem *newFocus, QGraphicsItem *oldFocus);
private:
    MyView* view;
    QImage *mImage;
signals:
    void activeTheSingleLineFuncion(int whichLine);
};

#endif // MYTEXTSCENE_H
