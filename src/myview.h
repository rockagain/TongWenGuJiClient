#ifndef MYVIEW_H
#define MYVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QScrollBar>

#include "config.h"
#include "myitem.h"

class MyView : public QGraphicsView
{
//    Q_OBJECT
public:
    MyView();
    int scrollvalue;
    QScrollBar *bar;
    int boardType;
    void setBoardType(int value);
    int viewEnterType;
    QCursor ViewCursor;
    QScrollBar *hbar;
    QScrollBar *vbar;
protected:
    void keyPressEvent(QKeyEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    //signals:
        //    void setEditBoardOnFire();
};

#endif // MYVIEW_H
