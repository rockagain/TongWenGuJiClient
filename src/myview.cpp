#include "myview.h"
#include <QKeyEvent>
/**
 * @brief  设置view大小    设置画布灰色  setMouseTracking
 */
MyView::MyView()
{
    //   scale(0.5, 0.5);
    //    this->setStyleSheet("background-color:grey;"
    //                      /*  "QScrollBar:vertical{"
    //                        "background:#dbdbdb;};"*/);
    //    QScrollBar* scrollbar = new QScrollBar;
    //    scrollbar->setStyleSheet("background-color:white;");
    //    this->setVerticalScrollBar(scrollbar);
    this->setMouseTracking(false);
    //this->setStyleSheet("background-color:red;");
    scrollvalue = 0;
    bar = this->verticalScrollBar();

}

void MyView::setBoardType(int value)
{
    boardType = value;
}



void MyView::keyPressEvent(QKeyEvent *event)
{

    if(boardType == PROOFBOARD || boardType == EDITBOARD) {
        hbar->blockSignals(true);
        vbar->blockSignals(true);
    }

    QGraphicsView::keyPressEvent(event);
    MyItem * focusedItem;

    if(boardType == CLUSTERPROOF) {

        if (event->key() == Qt::Key_Down) {
            //scrollvalue += 1;
            //        if(scrollvalue*80 > this->height() - 30){
            //           bar->setValue((scrollvalue - (this->height() - 20)/80 +1)*80);
            //        }
            //this->scene()->items()   这个地方好像也知道改怎么做了，就是设置当item 不在最下面的时候不执行下面的
            for(auto i:this->scene()->items()) {
                if(i->hasFocus()){
                    focusedItem = (MyItem *)i;
                }
            }
            if(focusedItem->scenePos().y() + 80 > this->height() + bar->value()) {
                int value = bar->value() + 80;
                bar->setValue(value);
            }
        }
        if(event->key() == Qt::Key_Up) {
            //scrollvalue -= 1;
            //int value = bar->value() - 80;
            for(auto i:this->scene()->items()) {
                if(i->hasFocus()){
                    focusedItem = (MyItem *)i;
                }
            }
            if(focusedItem->scenePos().y()  < bar->value() ) {
                int value = bar->value() - 80;
                bar->setValue(value);
            }
        }
    }

}

void MyView::keyReleaseEvent(QKeyEvent *event)
{

    if(boardType == PROOFBOARD || boardType == EDITBOARD) {
        hbar->blockSignals(false);
        vbar->blockSignals(false);
    }

}

void MyView::enterEvent(QEvent *event)
{
    (boardType == CLUSTERPROOF || boardType == FULLPICPROOF)?(ViewCursor.setShape(Qt::ArrowCursor)):(ViewCursor.setShape(Qt::CrossCursor));
    QApplication::setOverrideCursor(ViewCursor);
}

void MyView::leaveEvent(QEvent *event)
{
    ViewCursor.setShape(Qt::ArrowCursor);
    QApplication::setOverrideCursor(ViewCursor);
}

