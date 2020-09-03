#include "myitem.h"


/**
 * @brief 依次是：设置可选择，设置接收事件 ，设置位置可以移动 ，设置形状可以变化
 */

MyItem::MyItem()
{

    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    prepareGeometryChange();

    bbox = new hocr::bbox;
    itemNode = new HocrNode;
    mode = new ItemMode;
    checkPointSwitch = true;
    //这个bool值决定了是否在最后被检测
    isCheckOkOrNot = false;
}

/**
 * @brief 接收item的按钮事件
 * @param event
 */
void MyItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<< "isitemChangeOrNot:     " <<isItemChangeOrNot;

    qDebug()<< this->nodeMode;
    if(event->button() == Qt::LeftButton) {
        QGraphicsItem::mousePressEvent(event);
        if(checkPointSwitch){
            checkPoint();
            qDebug()<< "111";
            checkPointSwitch = false;
        }
        if((this->cursor.shape() == Qt::SizeFDiagCursor)||(this->cursor.shape() == Qt::SizeBDiagCursor))
        {
            //控制是否变形开关
            isItemChangeOrNot = true;
        }else{
            isItemChangeOrNot = false;
        }
        qDebug()<<this->flags();
        if(this->flags() == (QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsSelectable| QGraphicsItem::ItemIsMovable)) {
            //先把这个拿了试试
            if((this->cursor.shape() != Qt::SizeFDiagCursor)&&(this->cursor.shape() != Qt::SizeBDiagCursor)){
                cursor.setShape(Qt::OpenHandCursor);
                QApplication::setOverrideCursor(cursor);
            }
        }
    } else if(event->button() == Qt::RightButton) {
        event->ignore();
    }

}

/**
 * @brief 鼠标移动的事件，isitemchangeOrnot 是用来判断 item 是移动还是抻拉的  用这个来判断是否改变形状
 * @param event
 */

void MyItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);


    //变形的话就需要设置checkPoint 检测一下，我们这里默认是这样的
    this->setIsCheckOkOrNot(true);


    //拖动这个块变形
    if(isItemChangeOrNot) {
        this->setFlag(QGraphicsRectItem::ItemIsMovable, false);
        if(cursorLocation == rightUnder){
            cursor.setShape(Qt::SizeFDiagCursor);
            QApplication::setOverrideCursor(cursor);
            setRect(start.x(),start.y(), event->scenePos().x() - start.x()  ,  event->scenePos().y() - start.y());
        } else if(cursorLocation == rightUpper){
            cursor.setShape(Qt::SizeBDiagCursor);
            QApplication::setOverrideCursor(cursor);
            setRect(start.x(), event->scenePos().y(), event->scenePos().x() - start.x() , end.y() - event->scenePos().y());
        } else if(cursorLocation == leftUnder){
            cursor.setShape(Qt::SizeBDiagCursor);
            QApplication::setOverrideCursor(cursor);
            setRect(event->scenePos().x(),start.y(),end.x() - event->scenePos().x(),event->scenePos().y()-start.y());
        } else if(cursorLocation == leftUpper){
            cursor.setShape(Qt::SizeFDiagCursor);
            QApplication::setOverrideCursor(cursor);
            setRect(event->scenePos().x(),event->scenePos().y(),end.x() - event->scenePos().x(),end.y() - event->scenePos().y());
        }
        setItemAxis();
    } else {
        if(this->flags() != (QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsSelectable)) {
            this->setRect(start.x(),start.y(),rect().width() , rect().height());
            setItemAxis();
        }else{
            return;
        }
    }
    checkPointSwitch = true;
}

/**
 * @brief isitemchangeOrnot 是用来判断 item 是移动还是抻拉的  用这个来判断是否改变形状  如上
 * @param event
 */
void MyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    //    qDebug() << "------------------------------------------------------------";
    //    qDebug() << "startX:   " <<startX <<  "   startY:   "<< startY;
    //    qDebug() << "endX:   " <<endX << "   endY:  "<< endY;
    //    qDebug() << "start:   " <<start/* << "start:    "<< start*/;
    //    qDebug() << "-------------------------------------------------------------";
    //    QGraphicsItem::mouseReleaseEvent(event);
    //    isItemChangeOrNot = false;

    //
    //checkPoint();
    //    //定下来这个地方
    //    // 获取bbox的值
    //    //setItemAxis();
    //    if(this->mode->isDrag){
    //        this->setFlag(QGraphicsItem::ItemIsMovable);
    //    }
}

/**
 * @brief 判断在item上移动时候是否需要小箭头
 * @param event
 */
void MyItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    //这个地方写的不好，应该封装成一个函数
    setHoverCursorShapeAndState(event);
}

/**
 * @brief 恢复箭头的形状
 * @param event
 */
void MyItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    //  鼠标flag
    if(this->mode->isDeleteAble){
        cursor.setShape(Qt::CrossCursor);
        QApplication::setOverrideCursor(cursor);
    }
    //checkPoint();
}

int MyItem::type() const      //这个是为了选择删除而作准备的
{
    // qgraphicsitem_cast.
    return UserType + 1 ;
}

/**
 * @brief 检测item移动后的坐标
 */
void MyItem::checkPoint()
{
    //qreal a = 1.5;
    qDebug()<<"aaaa  boundingRect().x(): " << boundingRect().x();
    qDebug()<<"aaaa  sceneboundingRect().x(): " << sceneBoundingRect().x();
    qDebug()<<"aaaa  rect().x(): "<<rect().x();

    start.setX(sceneBoundingRect().x());
    start.setY(sceneBoundingRect().y());
    //    start.setX(rect().x());
    //    start.setY(rect().y());
    end.setX(start.x() + rect().width());
    end.setY(start.y() + rect().height());
    setItemAxis();
}

void MyItem::setIsDeleteAble(bool value)
{
    isDeleteAble = value;
}

void MyItem::setMode(int nodeType,bool ischange,bool isdrag,bool isSeletOrNot,bool isDeleteAble,int boardType)
{
    mode->nodeType = nodeType;
    mode->isChange = ischange;
    mode->isDrag = isdrag;
    mode->isSelectOrNot = isSeletOrNot;
    mode->isDeleteAble = isDeleteAble;
    mode->boardType = boardType;
}

/**
 * @brief setter
 * @param value
 */
void MyItem::setNodeMode(int value)
{
    nodeMode = value;
}

void MyItem::setBoardtype(int value)
{
    boardtype = value;
}


/**
 * @brief MyItem::getBbox
 * @return
 */
hocr::bbox *MyItem::getBbox() const
{
    return bbox;
}

/**
 * @brief MyItem::setBbox
 * @param value
 */
void MyItem::setBbox(hocr::bbox *value)
{
    bbox = value;
}

/**
 * @brief type函数是为了，区分item类型的
 * @return
 */

/**
 * @brief 获取bbox的值
 */
void MyItem::setItemAxis()
{
    //    bbox->right = sceneBoundingRect().x() + sceneBoundingRect().width() ;
    //    bbox->bottom = sceneBoundingRect().y() + sceneBoundingRect().height() ;
    //    bbox->left = sceneBoundingRect().x();
    //    bbox->top  = sceneBoundingRect().y();
    bbox->right = rect().x() + rect().width() ;
    bbox->bottom = rect().y() + rect().height() ;
    bbox->left = rect().x();
    bbox->top  = rect().y();
    //在这里直接赋值了，就不用这么麻烦了
    this->itemNode->SetBbox(bbox->left,bbox->top,bbox->right,bbox->bottom);
    //qDebug()<< bbox->left << bbox->top << bbox->right<< bbox->bottom;
}

//
void MyItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu = new QMenu((QWidget*)this->parentWidget());
    mAction = new QAction(QStringLiteral("删除"));
    menu->addAction(mAction);

    menu->setContextMenuPolicy(Qt::ActionsContextMenu);
    menu->move(event->screenPos().x(),event->screenPos().y());
    menu->show();

}

void MyItem::operator =(const MyItem &currentItem)
{
    this->mode = currentItem.mode;
}

void MyItem::setHoverCursorShapeAndState(QGraphicsSceneHoverEvent *event)
{
    if((event->scenePos().x() > (end.x()-10))
            && (event->scenePos().x() <= end.x())
            && (event->scenePos().y() > (end.y()-10))
            && ((event->scenePos().y() <= end.y()))
            && this->isSelected())
    {
        //设置鼠标flag
        cursor.setShape(Qt::SizeFDiagCursor);
        QApplication::setOverrideCursor(cursor);
        cursorLocation = rightUnder;
        if(this->mode->isDrag == true){
            this->setFlag(QGraphicsItem::ItemIsMovable,true);
        }

    } else if((event->scenePos().x() > (end.x()-10))
              && (event->scenePos().x() <= end.x())
              && (event->scenePos().y() > (end.y() - rect().height()))
              && (event->scenePos().y() <= (end.y() - rect().height()+10))
              && this->isSelected()){
        //设置鼠标flag
        cursor.setShape(Qt::SizeBDiagCursor);
        QApplication::setOverrideCursor(cursor);
        cursorLocation = rightUpper;
        if(this->mode->isDrag == true){
            this->setFlag(QGraphicsItem::ItemIsMovable,true);
        }
    } else if((event->scenePos().x() > start.x())
              && (event->scenePos().x() <= start.x()+10)
              && (event->scenePos().y() > start.y())
              && (event->scenePos().y() <= start.y()+10)
              && this->isSelected()){
        //设置鼠标flag
        cursor.setShape(Qt::SizeFDiagCursor);
        QApplication::setOverrideCursor(cursor);
        cursorLocation = leftUpper;
        if(this->mode->isDrag == true){
            this->setFlag(QGraphicsItem::ItemIsMovable,true);
        }
    } else if((event->scenePos().x() > start.x())
              && (event->scenePos().x() <= start.x()+10)
              && (event->scenePos().y() > start.y() + rect().height() -10)
              && (event->scenePos().y() <= start.y() + rect().height())
              && this->isSelected()){
        //设置鼠标flag
        cursor.setShape(Qt::SizeBDiagCursor);
        QApplication::setOverrideCursor(cursor);
        cursorLocation = leftUnder;
        if(this->mode->isDrag == true){
            this->setFlag(QGraphicsItem::ItemIsMovable,true);
        }
    } else {
        if(this->mode->isDeleteAble){
            cursor.setShape(Qt::ArrowCursor);
            QApplication::setOverrideCursor(cursor);
            cursorLocation = inThecenter;
        }
    }
}


void MyItem::keyPressEvent(QKeyEvent *event)
{

    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_S) {
        setSceneAndEditBoardOnFire();
    }
}

bool MyItem::getIsCheckOkOrNot() const
{
    return isCheckOkOrNot;
}

void MyItem::setIsCheckOkOrNot(bool value)
{
    isCheckOkOrNot = value;
}
