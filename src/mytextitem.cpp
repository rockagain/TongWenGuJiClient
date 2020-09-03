#include "mytextitem.h"

MyTextItem::MyTextItem()
{
    this->setNodemode(BIG_CHAR);
    hasPuncItem = false;
    //下面这句是非法的，
    //itemNode->beforePunc = false;
    //haspuncItem  true 和 false是判断 ，不管先new出来
    //this->puncNode= new HocrNode;
}

void MyTextItem::setItemSize(int size)
{
    QFont font = this->font();
    font.setPixelSize(size);
    this->setFont(font);
    mytextDialog = new MySingleTextDialog;  //这个地方是个

    connect(mytextDialog,&MySingleTextDialog::sentChangedWord,this,&MyTextItem::changeTextToWhatYourNodeSet);
}

void MyTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    mytextDialog->show();
    //mytextDialog->setGeometry(1920/3*2 + this->sceneBoundingRect().x() +20,this->sceneBoundingRect().y() + 27,222,180);
    mytextDialog->setGeometry(1920/3*2 + this->sceneBoundingRect().x() +20,560,222,180);
    if((1920/3*2 + this->sceneBoundingRect().x())>1920){
        mytextDialog->setGeometry(1920/3*2 + this->sceneBoundingRect().x() - 222,this->sceneBoundingRect().y()/2,222,180);
    }
}

void MyTextItem::setItemNode(HocrNode *value)
{
    itemNode = value;
}

HocrNode *MyTextItem::getItemNode() const
{
    return itemNode;
}

void MyTextItem::setCharAndPicList(CharAndCharRegionList *value)
{
    charAndPicLists = value;
}

void MyTextItem::setNodemode(int value)
{
    nodemode = value;
}

void MyTextItem::setBoardType(int value)
{
    boardType = value;
}

void MyTextItem::setPuncNode(HocrNode *value)
{
    puncNode = value;
}

//信号和槽   把检查过的颜色给改了  而且还把字给改了
void MyTextItem::changeTextToWhatYourNodeSet(QString text)
{
    //qDebug()<< text
    if (1 == text.count()) {
        this->setPlainText(text);
        //在这里更改node的信息
        this->getItemNode()->SetCharacter(text.toStdString());
    }
    this->setDefaultTextColor(QColor(94,213,209));
    // 下面这个是看字的位置的，果然在一开始构造函数那里是没分配位置的，在这里就可以了
    //qDebug()<< this->x() << "%%" <<this->y();
}

void MyTextItem::keyPressEvent(QKeyEvent *event)
{
    int a = event->key();
    if(event->key() == Qt::Key_Return && this->boardType == FULLPICPROOF) {
        mytextDialog->show();
        mytextDialog->showHasFocus();
    }
    //读取hocr然后重新给每个item赋值puncNode的时候，需要设置bool值 punctuation
    if(event->key() == Qt::Key_Comma && this->boardType == FULLPICPROOF) {
        this->hasPuncItem = true;
        this->puncNode= new HocrNode;
        puncNode->SetNodeClass("ocrx_word");
        QString q = QStringLiteral("，");
        string w = q.toStdString();
        puncNode->SetCharacter(w);
        //设置node的值，为的是记录，要不循环那里生成新的item不显示标点符号
        this->itemNode->hasPuncNode = true;
        this->itemNode->hasWhatPunc = w;

        qDebug() << QString::fromStdString(puncNode->GetCharacter());
        this->setPuncNode(puncNode);
        textSceneAddItemSlot();
    }
    if(event->key() == Qt::Key_Period && this->boardType == FULLPICPROOF) {
        this->hasPuncItem = true;
        this->itemNode->hasPuncNode = true;
        this->puncNode= new HocrNode;
        this->puncNode->SetNodeClass("ocrx_word");
        QString q = QStringLiteral("。");
        string w = q.toStdString();
        this->puncNode->SetCharacter(w);
        //设置node的值，为的是记录，要不循环那里生成新的item不显示标点符号
        this->itemNode->hasWhatPunc = w;
        qDebug() << QString::fromStdString(puncNode->GetCharacter());
        this->setPuncNode(puncNode);
        textSceneAddItemSlot();
    }
    if(event->key() == Qt::Key_Backslash && this->boardType == FULLPICPROOF) {
        this->hasPuncItem = true;
        this->itemNode->hasPuncNode = true;
        this->puncNode= new HocrNode;
        this->puncNode->SetNodeClass("ocrx_word");
        QString q = QStringLiteral("、");
        string w = q.toStdString();
        this->puncNode->SetCharacter(w);
        //设置node的值，为的是记录，要不循环那里生成新的item不显示标点符号
        this->itemNode->hasWhatPunc = w;
        qDebug() << QString::fromStdString(puncNode->GetCharacter());
        this->setPuncNode(puncNode);
        textSceneAddItemSlot();
    }
    if(event->key() == Qt::Key_Slash && this->boardType == FULLPICPROOF) {
        this->hasPuncItem = true;
        this->itemNode->hasPuncNode = true;
        this->puncNode= new HocrNode;
        this->puncNode->SetNodeClass("ocrx_word");
        QString q = QStringLiteral("、");
        string w = q.toStdString();
        this->puncNode->SetCharacter(w);
        //设置node的值，为的是记录，要不循环那里生成新的item不显示标点符号
        this->itemNode->hasWhatPunc = w;
        qDebug() << QString::fromStdString(puncNode->GetCharacter());
        this->setPuncNode(puncNode);
        textSceneAddItemSlot();
    }
    if(event->key() == Qt::Key_Delete && this->boardType == FULLPICPROOF) {
        this->hasPuncItem = false;
        this->itemNode->hasPuncNode = false;
        qDebug()<< "delete";
        textSceneAddItemSlot();
    }
    //顿号写在这里啊
    //    if(event->key() == Qt::Key_Period && this->boardType == FULLPICPROOF) {
    //        this->hasPunctuation = false;
    //        qDebug() << "juhao";
    //        HocrNode * puncNode= new HocrNode;
    //        puncNode->SetNodeClass("ocr_punc");
    //        puncNode->SetCharacter("。");
    //        this->setPuncNode(puncNode);
    //        this->showPuncItem(puncNode);
    //        Key_Slash     Key_Backslash
    //    }
}



void MyTextItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(this->boardType == CLUSTERPROOF){
        this->setDefaultTextColor(QColor(94,213,209));
    }
}





