#include "mytextscene.h"

MyTextScene::MyTextScene()
{
    connect(this,&QGraphicsScene::sceneRectChanged,this,&MyTextScene::checkSceneRect);
    connect(this,&MyTextScene::focusItemChanged,this,&MyTextScene::resetViewsScrollbarvalue);
    //这两个从此和 clusterproof没关系了，只和fullpicProof的上下键盘计数有关
    keyUpInt = 0;
    keyDownInt = 0;

    proof_wholeColumnNum = 0;     //一共多少行
    proof_WordNumPerColumn = 0;  //每行多少字
    proof_CurrentColumnIndex = 0;   //当前在那个行
}


void MyTextScene::checkSceneRect(const QRectF &rect)
{

}

void MyTextScene::setView(MyView *value)
{
    view = value;
}

void MyTextScene::setBoardType(int value)
{
    boardType = value;
}

void MyTextScene::testSceneRectToSeeIfItsRight()
{
    textItem = new MyTextItem();
    textItem->setFlag(QGraphicsTextItem::ItemIsFocusable);
    textItem->setPlainText(QStringLiteral("这"));

    //    textItem->setDefaultTextColor(Qt::red);

    textItem->setItemSize(50);
    textItem->setPos(0, 0);
    this->addItem(textItem);
    this->setSceneRect(0, 0, 1920/9*3, 900);
    words.append(textItem);
    textItem = new MyTextItem();
    textItem->setFlag(QGraphicsTextItem::ItemIsFocusable);
    textItem->setPlainText(QStringLiteral("个"));
    textItem->setItemSize(50);
    textItem->setPos(0, 50);
    this->addItem(textItem);

    words.append(textItem);
    textItem = new MyTextItem();
    textItem->setFlag(QGraphicsTextItem::ItemIsFocusable);
    textItem->setPlainText(QStringLiteral("个"));
    textItem->setItemSize(50);
    textItem->setPos(50, 0);

    this->addItem(textItem);
    words.append(textItem);

}

//鼠标事件 目前这里面是向上和向下移动的//下面这段代码写的太乱了
//代码首先封装好， 判断好， 要不然两个模块都混起来了，造成逻辑混乱
//聚类这里需要注意的点就是 ， 你增加或者减少的字，以后都需要 注意啊  要不以后就没法继续了
void MyTextScene::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);
    if (event->key() == Qt::Key_Down) {
        //把clustproof  和  fullpicproof  摘出来 ， 两个分开开 ， 其实if下面的东西应该封装
        if(this->boardType == CLUSTERPROOF) {
            if(keyPressdInt == 0)
                return;
            --keyPressdInt ;
            //            for(auto i : this->items()) {
            //                i->clearFocus();
            //            }
            //((MyTextItem*)(this->items().at(keyPressdInt)))->setDefaultTextColor(QColor(94,213,209));
            currentTextItem = (MyTextItem*)this->items().at(keyPressdInt);
            currentTextItem->setFocus();
            currentTextItem->setSelected(true);
        }

        //把clustproof  和  fullpicproof  摘出来 ， 两个分开开 ， 其实if下面的东西应该封装
        if(this->boardType == FULLPICPROOF) {
            if((keyDownInt <= words.size() -1))
                ++ keyDownInt ;
            if(keyDownInt == words.size()){
                //调用信号和槽去启动fullpicproof里面的调用单行的函数
                if(proof_CurrentColumnIndex < proof_wholeColumnNum){
                    ++ proof_CurrentColumnIndex;
                }
                if(proof_CurrentColumnIndex == proof_wholeColumnNum){
                    if(this->boardType == FULLPICPROOF)
                        proof_CurrentColumnIndex --; //写的不是很优雅啊。。。。。。。。。。。。。。太次了
                    return;
                }
                for(auto i : this->items()) {
                    i->clearFocus();
                }
                activeTheSingleLineFuncion(proof_CurrentColumnIndex);
                keyDownInt = 0;
            }
            //            this->items().at(words.size() - keyDownInt -1)->setFocus();
            //            ((MyTextItem*)(this->items().at(words.size() - keyDownInt -1)))->setDefaultTextColor(QColor(94,213,209));
            //            currentTextItem = (MyTextItem*)this->items().at(words.size() - keyDownInt -1);
            //            currentTextItem->setSelected(true);

            words.at(keyDownInt)->setFocus();
            words.at(keyDownInt)->setDefaultTextColor(QColor(94,213,209));
            currentTextItem = words.at(keyDownInt);
            currentTextItem->setSelected(true);
        }
    }

    /********************************************************************************/

    if(event->key() == Qt::Key_Up) {

        //把clustproof  和  fullpicproof  摘出来 ， 两个分开开 ， 其实if下面的东西应该封装
        if(this->boardType == CLUSTERPROOF) {
            if(keyPressdInt == items().size() - 1)  //这里应该有更好的，更合理的实现
                return;
            ++ keyPressdInt;

            //            for(auto i : this->items()) {
            //                i->clearFocus();
            //            }

            //((MyTextItem*)(this->items().at(keyPressdInt)))->setDefaultTextColor(QColor(94,213,209));
            currentTextItem = (MyTextItem*)this->items().at(keyPressdInt);
            currentTextItem->setFocus();
            currentTextItem->setSelected(true);
        }

        //把clustproof  和  fullpicproof  摘出来 ， 两个分开开 ， 其实if下面的东西应该封装
        if(this->boardType == FULLPICPROOF) {
            if((keyDownInt >= 0)&& !((proof_CurrentColumnIndex == 0)&&(keyDownInt == 0)))  //这里应该有更好的，更合理的实现
                keyDownInt --;
            //这个 -1 是妥协的写法，  我觉得正常的逻辑，应该是很顺的,
            if(keyDownInt == -1){
                //调用信号和槽去启动fullpicproof里面的调用单行的函数
                if(proof_CurrentColumnIndex > 0){
                    -- proof_CurrentColumnIndex;
                }
                //??????????????????这么写是不是对的
                //            if(proof_CurrentColumnIndex == -1){
                //                proof_CurrentColumnIndex = 0;
                //                return;
                //            }
                for(auto i : this->items()) {
                    i->clearFocus();
                }
                activeTheSingleLineFuncion(proof_CurrentColumnIndex);
                keyDownInt = words.size() - 1;
            }
            words.at(keyDownInt)->setFocus();
            words.at(keyDownInt)->setDefaultTextColor(QColor(94,213,209));
            currentTextItem = words.at(keyDownInt);
            currentTextItem->setSelected(true);
        }
    }
}

void MyTextScene::resetViewsScrollbarvalue(QGraphicsItem *newFocus, QGraphicsItem *oldFocus)
{
    if(boardType == CLUSTERPROOF){
        for(int i = 0; i< words.size(); i++){
            if(words.at(i)->hasFocus()){
                view->scrollvalue = words.size() - i - 1;
            }
        }
        //       // currentTextItem = (MyTextItem*)oldFocus;
    }
    //    if(boardType == FULLPICPROOF && newFocus){
    //        ((MyTextItem*)oldFocus)->mytextDialog->hide();
    //    }
}

/**
 * @brief MyTextScene::setImage  没用了，用不上
 * @param image
 */
void MyTextScene::setImage(QImage *image)
{
    mImage = image;
}


void MyTextScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if (items(event->scenePos()).size() != 0) {
        //很好！！！！！
        qDebug() << "I comming";
        currentTextItem = (MyTextItem*)items(event->scenePos()).at(0);
        currentTextItem->setSelected(true);
        currentTextItem->setFocus();
        //keyDownInt = items().size() - items().indexOf(currentTextItem) -1;
        //下面这个和整页校对那里相关
        keyDownInt = items().size() - items().indexOf(currentTextItem) - 2;
        //下面这个和聚类校对那里相关
        keyPressdInt = items().indexOf(currentTextItem);
        qDebug() <<keyPressdInt << "%%";
        qDebug() <<items().indexOf(currentTextItem) << "index";
        qDebug() << this->items().size() << "&&";
    }
}


void MyTextScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    for(auto i : this->items()){
        //  if(i->contains(event->scenePos())){
        //   ((MyTextItem*)i)->mytextDialog->hide();
        //            break;
        //        }
    }
    QGraphicsScene::mouseDoubleClickEvent(event);
}


void MyTextScene::showPuncItem()
{
    for(auto i : this->items()) {
        if(((MyTextItem * )i)->nodemode == PUNC)
            this->removeItem(i);
    }
    for(MyTextItem * i: words) {
        if(i->hasPuncItem == true) {
            tempTextItem = new MyTextItem;
            tempTextItem->setNodemode(PUNC);
            // QString q;
            // q = QString::fromLocal8Bit("、");
            // string s = q.toStdString();
            // qDebug() << QString::fromStdString(s);

            string q = i->puncNode->GetCharacter();
            //            qDebug() << QString::fromLocal8Bit(q.c_str(), strlen(q.c_str()));
            //            QString w = QString::fromLocal8Bit(q.c_str(), strlen(q.c_str()));
            QString w = QString::fromStdString(q);

            tempPuncNode = new HocrNode;
            tempPuncNode->SetCharacter(q);
            hocr::bbox box = i->getItemNode()->GetBbox();
            tempPuncNode->SetBbox(box.left,box.top + 1,box.right,box.bottom);
            i->puncNode = tempPuncNode;
            //这个QVector


            if(w == QString(QStringLiteral("，"))){
                tempTextItem->setPlainText(QStringLiteral("，"));
            }

            if(w == QString(QStringLiteral("。"))){
                tempTextItem->setPlainText(QStringLiteral("。"));
            }

            if(w == QString(QStringLiteral("、"))){
                tempTextItem->setPlainText(QStringLiteral("、"));
            }
            this->addItem(tempTextItem);
            //这里是把标点符号给加上去，坐标是根据item的pos调的
            tempTextItem->setPos(i->scenePos().x() + 30,i->scenePos().y() + 10 );
        }
    }
}


