#ifndef MYSCENE_H
#define MYSCENE_H

#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QImage>
#include <QBrush>
#include <QColor>
#include <QGraphicsRectItem>
#include <QAction>



#include "myitem.h"
#include "config.h"
#include "libhocr/hocr_types.h"
#include "libhocr/hocr_node.h"
#include "libhocr/tinyxml2/tinyxml2.h"
#include "networkclub.h"
#include "mypixmapitem.h"
#include "mytextitem.h"


using namespace hocr;

class MyScene : public QGraphicsScene
{
    Q_OBJECT

public:   
    MyScene();
    void setMode(const int &value);
    void setView(QGraphicsView *value);
    void setColorToCheck();
    bool colorOrNot; // 设置这个函数而得到 是否染色查询 或者 透明
    void setPenColorAndMode(int colorMode);
    void getHocrToGo();

    //设置这里的image 成申请下来的image
    void setImage(QImage *image);

    //虚构函数，假如对scene  的各种有需求的话，那就去实现它
    void  drawBackground(QPainter *painter, const QRectF &rect);

    QPen pen;
    void setBoardtype(int value);

    QPointF start;
    QPointF end;

    bool isCreateOrIsHover;
    //这里也需要resize
    //    void resizeEvent(QResizeEvent *event);
    QCursor SceneCursor;
    int mode;
    int nodeMode;

    int getBoardtype() const;
    MyItem* rectItem;
    MyItem* copyedItem;

private:
    bool drawSwitch;
    QGraphicsView *view;
    bool pasteOrNot;

    void mousePressEvent(QGraphicsSceneMouseEvent  *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent  *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    //添加右键菜单的
    //右键删除的
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QMenu* menu;
    QAction* deleteAction;
    QAction* pasteAction;
    void setItemDeleted();
    QPointF actionPoint;

    QImage *mImage;
    MyItem *tempItem;
    void zoomScene();

    //画笔边框

    //默认一个node父节点
    HocrNode *pageNode;
    HocrNode *careaNode;

    void makeNode(MyItem *rectItem,QGraphicsSceneMouseEvent  *event);
    //左边坐标变换左边显示item
    MyItem* leftTempItem;
    //板子的类型
    int boardtype;
    //聚类校对时候用的的uperScene的图片
    MyPixmapItem* pixmapitem;
    //处理聚类校对时候的图片
    QVector<MyPixmapItem*> pixmapitems;
    void handlePicture(QVector<MyPixmapItem*> pixmapitems);
    void checkToSeeAndDelete(MyItem *item);
    QGraphicsTextItem* leftLastItem;
    MyItem* pressedCurrentItem;

public slots:
    void focusItemChangedResponse(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem);
    void clusterLeftSceneShow(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem);
signals:
    void setClusterProofsLineEditFocus();
    void setBoardOnFire();  //这个函数是调用editboard里面的保存函数的
protected:
    void keyPressEvent(QKeyEvent *event);
private slots:
    void copyOktoPaste();
};

#endif // MYSCENE_H







































