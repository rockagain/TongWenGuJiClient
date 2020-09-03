#include "myscene.h"
using namespace tinyxml2;


/**
 * @brief 读取图片  得到数据
 */
MyScene::MyScene()
{
    colorOrNot = true;   //todo .  应该是遮盖检查的开关，一会找出来
    mImage = new QImage;

    //设置根节点
    pageNode = new HocrNode;
    pageNode->SetNodeClass("ocr_page");
    pageNode->SetNodeId("page_1");
    pageNode->SetBbox(0,0,mImage->width(),mImage->height());
    pageNode->SetPhysicPageNo("ppageno 0");
    //先赋值，后期判断
    careaNode = nullptr;
    //删除
    deleteAction = new QAction(QStringLiteral("删除"));
    pasteAction = new QAction(QStringLiteral("黏贴"));
    rectItem = new MyItem;

    connect(deleteAction, &QAction::triggered, this,&MyScene::setItemDeleted);
    connect(pasteAction,&QAction::triggered,this,&MyScene::copyOktoPaste);
    connect(rectItem,&MyItem::setSceneAndEditBoardOnFire,this,&MyScene::setBoardOnFire);
    leftLastItem = nullptr;
    //这个参数是防止崩溃的，是画方框那里的
    drawSwitch = false;

}

/**
 * @brief need to delete
 * @param value
 */
void MyScene::setView(QGraphicsView *value)
{
    view = value;
}

/**
 * @brief 设置mode 是为了区分状态
 * @param value
 */
void MyScene::setMode(const int &value)
{
    mode = value;
}

/**
 * @brief 鼠标按压事件，里面有删除和画图的操作
 * @param event
 */
void MyScene::mousePressEvent(QGraphicsSceneMouseEvent  *event)
{
    QGraphicsScene::mousePressEvent(event);
    //这个参数的名字在一开始的时候起的不是很好，有些误导性
    if(!colorOrNot) {
        for(auto i : this->items()) {
            //防崩溃
            if(this->boardtype == CLUSTERPROOF)
                break;
            if(!i->isSelected()){
                i->setFlag(QGraphicsItem::ItemIsMovable,false);
            }
        }
    } else  {
        //下面是取消选中 和 上颜色的
        for(auto i : this->items()) {
            //防崩溃
            if(this->boardtype == CLUSTERPROOF)
                break;
            if(!i->isSelected()){
                i->setFlag(QGraphicsItem::ItemIsMovable,false);
                i->setOpacity(1);
                //下面这一个地方是验证是否在涂黑状态的，假如是黑色的，那么就不搞这个那个的了其实就一句
                //            if(((MyItem*)i)->brush().color() == Qt::black)
                //                return;
                //            //**********************************************
                ((MyItem*)i)->setBrush(Qt::transparent);
            }
        }
        for(auto i : this->items(event->scenePos())) {
            //防崩溃
            if(this->boardtype == CLUSTERPROOF)
                break;
            if(i->isSelected() ) {
                ((MyItem*)i)->setBrush(Qt::blue);
                ((MyItem*)i)->setOpacity(0.2);
            }
        }
    }

    QPointF pointer;
    pointer.setX(0);
    pointer.setY(0);
    start = pointer;
    if(event->button() == Qt::LeftButton) {
        //下面这个函数是管伸缩图片的，暂时先注释
        //zoomScene();
        start = event->scenePos();
    }
    //画图开始
    if(event->button() == Qt::LeftButton && mode == ManageMode::EDIT && !event->isAccepted()) {
        tempItem = new MyItem;
        addItem(tempItem);
        isCreateOrIsHover = true;
        SceneCursor.setShape(Qt::CrossCursor);
        QApplication::setOverrideCursor(SceneCursor);
        drawSwitch = true;
    } else if (event->button() == Qt::RightButton) {
        //已删除
    }

}

/**
 * @brief  双击调用  zoomScene  放大缩小
 * @param event
 */
void MyScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        //下面这个函数是管伸缩图片的，暂时先注释
        //zoomScene();
    }
}

/**
 * @brief 鼠标移动事件  删除重绘图形
 * @param event
 */
void MyScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    //重新画
    if(start.x() == 0 && start.y() == 0) {
        return;
    }
    end = event->scenePos();
    if (/*event->button() == Qt::LeftButton  &&*/ mode == ManageMode::EDIT && !event->isAccepted() && isCreateOrIsHover) {
        //这个判断的意思是不能随便就创建了，谁知道为什么点击一下就可以这么搞了，不点击就可以啊
        if(!drawSwitch)
            return;
        if((start.x() >0)&&(start.y() > 0)) {
            removeItem(tempItem);
            tempItem->setPen(pen);
            tempItem->setRect(start.x(),
                              start.y(),
                              end.x() - start.x(),
                              end.y() - start.y());
            addItem(tempItem);

            //生成  和   hover 的开关

        } else {
            isCreateOrIsHover = false;
        }
    }
}

/**
 * @brief 鼠标release事件，画的最后一笔
 * @param event
 */
void MyScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //事件穿透影响画框子了，莫名其妙，先注释了，看看有没有影响
    //加上后面这个EDITBOARD 的这个是因为不加  它的框子走的
    if(this->boardtype == CLUSTERPROOF /* || this->boardtype == EDITBOARD || this->boardtype == PROOFBOARD*/) {
        QGraphicsScene::mouseReleaseEvent(event);
    }

    end = event->scenePos();
    if((event->button() == Qt::LeftButton)&&(mode == ManageMode::EDIT)&&!event->isAccepted() &&
            (start.x() >0)&&(start.y() > 0) && isCreateOrIsHover) {
        // add a custom item to the scene
        if(!drawSwitch)
            return;
        removeItem(tempItem);
        //这个判断是为了杜绝画太小的框子，画太小的框子看不见，引起混乱
        if(((end.x() - start.x())>20.0)&&((end.y() - start.y())>20.0)){
            rectItem = new MyItem;
            if(end.x() > this->width()){
                end.setX(this->width());
            }
            if(end.y() > this->height()){
                end.setY(this->height());
            }
            rectItem->setRect(start.x(),
                              start.y(),
                              end.x() - start.x(),
                              end.y() - start.y());
            //添加版子类型，要么就不中
            rectItem->setBoardtype(this->boardtype);
            //设置Item的边框颜色
            rectItem->setPen(pen);
            //设置Item的mode
            rectItem->setNodeMode(this->nodeMode);
            //获取bbox的值
            //            if(rectItem->nodeMode == MAIN_BODY){
            //                qDebug()<< "mainBodyItem left:  " <<rectItem->rect().left();
            //                qDebug()<< "mainBodyItem top:  " <<rectItem->rect().top();
            //                qDebug()<< "mainBodyItem right:  " <<rectItem->rect().right();
            //                qDebug()<< "mainBodyItem bottom:  " <<rectItem->rect().bottom();
            //            }
            rectItem->setItemAxis();
            //规避，按理说  bottom 要比  top 大 ，经测试，目前无发现错误
            if(rectItem->getBbox()->bottom< rectItem->getBbox()->top){
                return;
            }
            rectItem->mode->nodeType = this->nodeMode;
            rectItem->mode->isChange = false;
            rectItem->mode->isDrag = false;
            rectItem->mode->isSelectOrNot = false;
            rectItem->mode->isDeleteAble = true;
            rectItem->mode->boardType = this->boardtype;
            //根据Item类型，生成Node
            //最终定稿
            rectItem->setFlag(QGraphicsRectItem::ItemIsMovable,false);
            rectItem->setFlag(QGraphicsRectItem::ItemIsSelectable);
            //isCheckOk
            rectItem->setIsCheckOkOrNot(true);
            addItem(rectItem);
            //添加的if语句的意思是不在edit模式下，可能这种方式是没用的
            if (this->boardtype != PROOFBOARD) {
                //这个地方是生成node和item的地方  是最初画版模块识别item 生成hocr的地方
                //makeNode(rectItem,event);
            }
        }
    }
    this->views().first()->update();
    isCreateOrIsHover = false;
    setClusterProofsLineEditFocus();
    //    QPointF pointer;
    //    pointer.setX(0);
    //    pointer.setY(0);
    //    start = pointer;
    for(auto i : this->items()) {
        if(i->isSelected() && (this->boardtype != CLUSTERPROOF)){
            i->setFlag(QGraphicsItem::ItemIsMovable);
            //((MyItem*)i)->setItemAxis();
            //((MyItem*)i)->checkPoint();
            //            ((MyItem*)i)->setRect(((MyItem*)i)->start.x(),((MyItem*)i)->start.y(),
            //                                  ((MyItem*)i)->rect().width(),
            //                                  ((MyItem*)i)->rect().height());
            //qDebug() << start.x()<< " : " <<start.y() ;
        }
    }
}

/**
* @brief 放大缩小scene，即图片
*/
void MyScene::zoomScene()
{
    if(mode == ManageMode::ZOOMIN){
        views().first()->scale(1.1,1.1);
    }

    if(mode == ManageMode::ZOOMOUT){
        views().first()->scale(0.9,0.9);
    }
}

/**
 * @brief 虚函数重实现
 * @param painter
 * @param rect
 */
void MyScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    // todo:
    painter->drawImage(QRect(0, 0, mImage->width(), mImage->height()), *mImage);
    //painter->drawImage(QRect(0, 0, this->views().first()->width(), this->views().first()->width()), *mImage);
}

/**
 * @brief 设定颜色，然后去检查
 */
void MyScene::setColorToCheck()
{

    MyItem *tempItem = new MyItem;
    for (int i = 0; i < items().size(); i++) {
        tempItem = (MyItem *) items().at(i);
        //当板子是画版的时候
        if(this->boardtype == EDITBOARD){
            if(colorOrNot) {
                if(tempItem->nodeMode != MAIN_BODY) {
                    tempItem->setBrush(QBrush(Qt::black));
                    tempItem->setOpacity(1);
                }
            } else {
                if(tempItem->nodeMode != MAIN_BODY) {
                    tempItem->setBrush(QBrush(Qt::transparent));
                }
            }
        }
        //当板子是版面分析的时候
        if(this->boardtype == PROOFBOARD){
            if(colorOrNot) {
                if(tempItem->nodeMode == BIG_CHAR || tempItem->nodeMode == SMALL_CHAR) {
                    tempItem->setBrush(QBrush(Qt::black));
                    tempItem->setOpacity(1);
                }
            } else {
                if(tempItem->nodeMode == BIG_CHAR || tempItem->nodeMode == SMALL_CHAR) {
                    tempItem->setBrush(QBrush(Qt::transparent));
                }
            }
        }
    }
    //设置逆转每一个颜色
    colorOrNot =  !colorOrNot;
    //加上这句，来处理刷新的问题，没问题咱们不留着了，出了问题咱们就删除**
    this->update();
}

/**
 * @brief 改变item的 外沿色彩
 * @param colorMode
 */
void MyScene::setPenColorAndMode(int colorMode)
{
    //把笔设置的粗一点
    pen.setWidth(2);
    this->setFocus();
    switch (colorMode) {
    case DEFAULT:
        pen = QPen(Qt::darkBlue);
        nodeMode = DEFAULT;
        break;
    case SINGLE_ROW:
        //pen = QPen(QColor(218,249,102));
        pen = QPen(QColor(235,97,0));
        nodeMode = SINGLE_ROW;
        //把笔设置的粗一点
        pen.setWidth(2);
        this->setFocus();
        break;
    case DOUBLE_ROW:
        pen = QPen(QColor(230,0,18));
        nodeMode = DOUBLE_ROW;
        pen.setWidth(2);
        break;
    case BIG_CHAR:
        //pen = QPen(QColor(234,207,2));
        pen = QPen(QColor(9,124,37));
        nodeMode = BIG_CHAR;
        //把笔设置的粗一点
        pen.setWidth(1);
        this->setFocus();
        break;
    case SMALL_CHAR:
        pen = QPen(QColor(0,5,230));
        nodeMode = SMALL_CHAR;
        break;
    case WORD:
        pen = QPen(Qt::magenta);
        nodeMode = WORD;
        break;
    case IMAGE:
        pen = QPen(Qt::gray);
        nodeMode = IMAGE;
        pen.setWidth(2);
        break;
    case TABLE:
        pen = QPen(Qt::darkBlue);
        nodeMode = TABLE;
        pen.setWidth(2);
        break;
    case ANNOTATION:
        pen = QPen(Qt::darkCyan);
        nodeMode = ANNOTATION;
        pen.setWidth(2);
        break;
    case MAIN_BODY:
        pen = QPen(QColor(0,255,255));
        //pen = QPen(QColor(230,0,18));
        nodeMode = MAIN_BODY;
        pen.setWidth(2);
        break;
    default:
        break;
    }
}

/**
 * @brief 生成 careaNode 或者 子Node
 * @param item
 */
void  MyScene::makeNode(MyItem * item,QGraphicsSceneMouseEvent  *event)
{
    if(item->nodeMode == MAIN_BODY) {
        item->itemNode->SetParent(pageNode);
        careaNode = item->itemNode;
        careaNode->SetNodeClass("ocr_carea");
        careaNode->SetNodeId("block_1_1");
        item->setItemAxis();
        careaNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
        item->setFlag(QGraphicsItem::ItemIsMovable,false);
    } else if(item->nodeMode == SINGLE_ROW ) {
        //这里还需要处理再处理， 你先画单行，然后点提交，程序就崩溃了，因为没有做这一步的处理
        if(careaNode != nullptr) {
            careaNode->AddChild(item->itemNode);
            item->itemNode->SetParent(careaNode);
            item->itemNode->SetNodeClass("ocr_line");
            item->setItemAxis();
            item->itemNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
        }
    }
}

int MyScene::getBoardtype() const
{
    return boardtype;
}

void MyScene::setBoardtype(int value)
{
    boardtype = value;
}

/**
 * @brief 把各种 Node 组建成 hocr
 */
void MyScene::getHocrToGo()
{
    //防崩溃大法，你一个对象都没有，所以崩溃
    //    if(items().size() == 0)
    //        return;

    /*****************************************************************************************************************/
    //    下面是第一版的识别node，  自己写的，感觉写的不是很好，干脆，直接用leader写的模块去搞
    //    //申请doc 加上插入第一个page node
    //    XMLDocument doc;
    //    XMLElement* root = doc.NewElement("div");
    //    root->SetAttribute("class",pageNode->GetNodeClass().c_str());
    //    root->SetAttribute("id",pageNode->GetNodeId().c_str());
    //    std::string pageNodeBbox = "bbox 0 0 " + std::to_string(mImage->width()) +  " " + std::to_string(mImage->height());
    //    root->SetAttribute("title",pageNodeBbox.c_str());
    //    doc.InsertEndChild(root);

    //    //插入第二个节点即carea node
    //    XMLElement* careand = doc.NewElement("div");
    //    careand->SetAttribute("class",careaNode->GetNodeClass().c_str());
    //    careand->SetAttribute("id",careaNode->GetNodeId().c_str());
    //    std::string careaNodeBbox = "bbox " + std::to_string(careaNode->GetBbox().left) +  " " + std::to_string(careaNode->GetBbox().top) +  " " + std::to_string(careaNode->GetBbox().right) + " " +  std::to_string(careaNode->GetBbox().bottom);
    //    careand->SetAttribute("title",careaNodeBbox.c_str());
    //    root->InsertEndChild(careand);
    //    //插入line 子节点
    //    for(int i = 0 ; i < careaNode->GetChildren().size(); i++) {
    //        XMLElement* linenode = doc.NewElement("span");
    //        linenode->SetAttribute("class","ocr_line");
    //        //算出linenodeBbox 的 string
    //        HocrNode * node = careaNode->GetChildren().at(i);
    //        std::string linenodeBbox = "bbox " + std::to_string(node->GetBbox().left) + " " + std::to_string(node->GetBbox().top) + " " + std::to_string(node->GetBbox().right) + " " +  std::to_string(node->GetBbox().bottom);
    //        linenode->SetAttribute("title",linenodeBbox.c_str());
    //        linenode->InsertEndChild(doc.NewText(""));
    //        careand->InsertEndChild(linenode);
    //    }
    //    //doc.Print();
    //    XMLPrinter printer;
    //    doc.Print(&printer);
    //    //发送hocr 给 画版， 画版然后发送
    //    QString str = QString(printer.CStr()).replace('"','\"');
    /******************************************************************************************************************/
    //把item生成node

    //把node生成hocr

    //todo： 这里以后需要处理一下，空格这里需要处理
    //    str = str.replace("\n","").toHtmlEscaped();

    //    //把 hocr 发送给画版
    //    sceneSendHocr(str);
}

////action 动作触发删除Item

void MyScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu = new QMenu();
    menu->addAction(deleteAction);
    menu->addAction(pasteAction);

    menu->setContextMenuPolicy(Qt::ActionsContextMenu);
    menu->move(event->screenPos().x(),event->screenPos().y());
    menu->show();
    actionPoint = event->scenePos();
}

void MyScene::setItemDeleted()
{
    for(int i = 0; i < items(actionPoint).size(); i++) {
        checkToSeeAndDelete((MyItem*)(items(actionPoint).at(i)));
    }
}

void MyScene::setImage(QImage *image)
{
    mImage = image;
}

//右边的字被选择之后的变化，和左边的变化
void MyScene::focusItemChangedResponse(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem)
{
    //qDebug()<< "00000000000";
    if(oldFocusItem) {
        ((QGraphicsTextItem*)oldFocusItem)->setDefaultTextColor(QColor(94,213,209));
        this->removeItem(leftTempItem);
    }
    if(newFocusItem) {
        //        ((QGraphicsTextItem*)newFocusItem)->setDefaultTextColor(Qt::red);
        leftTempItem = new MyItem();
        QPen red = QPen(Qt::red);
        red.setWidth(2);
        leftTempItem->setPen(red);
        QPointF pos = newFocusItem->scenePos();
        QPointF pos1;
        //        pos1.setX(pos.x() - ((MyTextItem*)newFocusItem)->x);  //调两边坐标的地方
        //        pos1.setY(pos.y() - ((MyTextItem*)newFocusItem)->y);
        pos1.setX(((MyTextItem*)newFocusItem)->x);  //调两边坐标的地方
        pos1.setY(((MyTextItem*)newFocusItem)->y);
        //QRectF  rect = newFocusItem->boundingRect();
        leftTempItem->setRect(pos1.x(),pos1.y(),((MyTextItem*)newFocusItem)->w,((MyTextItem*)newFocusItem)->h);
        //        leftTempItem->setPos(pos1);

        this->addItem(leftTempItem);
    }
}
//在聚类校对那里
void MyScene::clusterLeftSceneShow(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem)
{
    if(this->boardtype == CLUSTERPROOF) {
        if(oldFocusItem) {
            ((QGraphicsTextItem*)oldFocusItem)->setDefaultTextColor(Qt::gray);
            //this->clear();
            //pixmapitems.clear();
        }

        if(newFocusItem) {
            leftLastItem = ((QGraphicsTextItem*)newFocusItem);
            this->clear();
            pixmapitems.clear();
            ((QGraphicsTextItem*)newFocusItem)->setDefaultTextColor(QColor(94,213,209));
            for(CharPicInfo* picinfo : ((*((MyTextItem*)newFocusItem)).charAndPicLists->regionList)) {
                pixmapitem = new MyPixmapItem;
                QPixmap pixmap;
                QImage _image = picinfo->image;
                //qDebug() << _image.byteCount();
                pixmapitem->setPixmap((pixmap.fromImage(picinfo->image)).scaled(100,100));
                //给这个图片附上信息
                pixmapitem->setPicInfo(picinfo);
                pixmapitem->setFlag(QGraphicsItem::ItemIsSelectable);
                this->addItem(pixmapitem);
                //把当前字设置在图片上
                pixmapitem->setCurrentWord((*((MyTextItem*)newFocusItem)).charAndPicLists->character);
                pixmapitems.append(pixmapitem);
            }
            this->handlePicture(pixmapitems);
        }

        if (leftLastItem != nullptr) {
            leftLastItem->setDefaultTextColor(QColor(94,213,209));
        }
    }
}

/**
 * @brief MyScene::handlePicture  从聚类校对板子那里借来的，处理图片距离的
 * @param pixmapitems
 */
void MyScene::handlePicture(QVector<MyPixmapItem*> pixmapitems)
{
    //这个转换是为了后面的一致性
    QVector<MyPixmapItem*>  pixmapitemsRevert;
    for (MyPixmapItem* i : pixmapitems) {
        pixmapitemsRevert.insert(0,i);
    }

    int row = 0;
    int column = 0;
    for(int i = 0; i < pixmapitemsRevert.size(); i++, column++) {
        if ((column - 1) *110 < this->width()) {
            pixmapitemsRevert.at(i)->setPos(column * 110, row * 110);
        }
        if ((column + 2)*110 > this->width()) {
            row += 1;
            column = -1;
        }
    }
    if(row * 100 < this->views().first()->height()) {
        this->setSceneRect(0,0,this->views().first()->width(),this->views().first()->height());
    } else {
        this->setSceneRect(0,0,this->views().first()->width(),row * 110 + 100);
    }
}

///**  好像和那边冲突了
// * @brief resizeEvent 移动了后都要resize一下
// * @param event
// */
//void MyScene::resizeEvent(QResizeEvent *event)
//{
//    if(boardtype == CLUSTERPROOF) {
//        this->setSceneRect(0,0,this->views().first()->width() - 10,this->views().first()->height() - 10);
//        handlePicture(pixmaptems);
//    }

//}

/**
 * @brief MyScene::checkToSeeAndDelete  查看是否可以删除
 * @param item
 */
void MyScene::checkToSeeAndDelete(MyItem* item)
{
    if(item->mode->isDeleteAble) {
        removeItem(item);
    }
}

/**
 * @brief MyScene::keyPressEvent   这里做的是上下左右移动item的方向
 * @param event
 */
void MyScene::keyPressEvent(QKeyEvent *event)
{

    //下面这一行的意思就是接受view传过来的键盘时间
    QGraphicsScene::keyPressEvent(event);
    static bool copyItem;
    bool isOk = false;
    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_V) {
        if(pasteOrNot == false)
            copyItem = pasteOrNot;
        pasteOrNot = copyItem;
        if(!copyItem)
            return;
        //this copyedItem is Key_C's Item

        copyedItem->setRect(pressedCurrentItem->sceneBoundingRect().x() + 10, pressedCurrentItem->sceneBoundingRect().y() + 10, pressedCurrentItem->rect().width(), pressedCurrentItem->rect().height());
        //下面这一行设置一下
        copyedItem->setIsCheckOkOrNot(true);
        this->addItem(copyedItem);
        //代码有冗余。。
        this->setPenColorAndMode(copyedItem->mode->nodeType);
        copyedItem->setNodeMode(copyedItem->mode->nodeType);
        copyedItem->setPen(this->pen);
        copyedItem->setFlag(QGraphicsItem::ItemIsMovable);
        pressedCurrentItem->setSelected(false);
        copyedItem->setFlag(QGraphicsItem::ItemIsSelectable);
        copyedItem->setSelected(true);
        copyedItem->setItemAxis();

        copyItem = false;
        pasteOrNot = false;
    }

    for(auto i:this->items()) {
        if(i->isSelected()) {
            pressedCurrentItem = (MyItem*)i;
            isOk = true;
        } else {
            continue;
        }
    }

    if(!isOk){
        return;
    }


    if (event->key() == Qt::Key_Down) {
        pressedCurrentItem->setY(pressedCurrentItem->y() + 3);
        pressedCurrentItem->setIsCheckOkOrNot(true);
    } else if(event->key() == Qt::Key_Up) {
        pressedCurrentItem->setY(pressedCurrentItem->y() - 3);
        pressedCurrentItem->setIsCheckOkOrNot(true);
    } else if(event->key() == Qt::Key_Left) {
        pressedCurrentItem->setX(pressedCurrentItem->x() - 3);
        pressedCurrentItem->setIsCheckOkOrNot(true);
    } else if(event->key() == Qt::Key_Right) {
        pressedCurrentItem->setX(pressedCurrentItem->x() + 3);
        pressedCurrentItem->setIsCheckOkOrNot(true);
    }


    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_C) {
        copyItem = true;
        pasteOrNot = true;
        copyedItem = new MyItem;
        *copyedItem = *pressedCurrentItem;
    }

    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_S) {
        setBoardOnFire();
    }
    if(event->key() == Qt::Key_Delete){
        this->removeItem(pressedCurrentItem);
    }

    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_D) {
        //this->clear();
        qDebug()<<"chufale";
        for(auto i:this->items()) {
            qDebug()<< ((MyItem*)i)->nodeMode;
            ((MyItem*)i)->setItemAxis();
            qDebug()<<((MyItem*)i)->isDeleteAble  << "    bottom  :"<<((MyItem*)i)->getBbox()->bottom << "   head :" <<((MyItem*)i)->getBbox()->top;
            //是这样的，假如bottom 比  head 小 那就赶快删除它吧 ,尝试再画的时候判断一下 ，然后就不会有这种现象了？ 做下测试
            if(((MyItem*)i)->getBbox()->bottom < ((MyItem*)i)->getBbox()->top) {
                this->removeItem(i);
            }
            //删除
            if(((MyItem*)i)->nodeMode == DEFAULT  || ((MyItem*)i)->nodeMode > 10){
                //qDebug()<< ((MyItem*)i)->nodeMode;
                this->removeItem(i);
            }
        }
    }
    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_Delete) {
        //this->clear();
        qDebug()<<"chufale";
        for(auto i:this->items()) {
            if(  (((MyItem*)i)->nodeMode != MAIN_BODY) || (((MyItem*)i)->nodeMode != SINGLE_ROW)  ){
                qDebug()<< ((MyItem*)i)->nodeMode;
                this->removeItem(i);
            }
        }
    }
}

/**
 * @brief MyScene::copyOktoPaste 复制后去黏贴
 */
void MyScene::copyOktoPaste()
{
    if(!pasteOrNot)
        return;
    //this copyedItem is Key_C's Item

    copyedItem->setRect(actionPoint.x() , actionPoint.y(), pressedCurrentItem->rect().width(), pressedCurrentItem->rect().height());
    this->addItem(copyedItem);
    //代码有冗余。。
    this->setPenColorAndMode(copyedItem->mode->nodeType);
    copyedItem->setNodeMode(copyedItem->mode->nodeType);
    copyedItem->setPen(this->pen);
    copyedItem->setFlag(QGraphicsItem::ItemIsMovable);
    pressedCurrentItem->setSelected(false);
    copyedItem->setFlag(QGraphicsItem::ItemIsSelectable);
    copyedItem->setSelected(true);
    copyedItem->setItemAxis();
    copyedItem->setIsCheckOkOrNot(true);
    pasteOrNot = false;
}
