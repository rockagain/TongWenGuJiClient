#include "itemrecognizer.h"
#include <iostream>
#include <QtAlgorithms>
using namespace tinyxml2;

/**
 * @brief itemrecognizer 也是 itemconstructer  既能识别item 也能构成item
 * @param sceneAtWork
 */

ItemRecognizer::ItemRecognizer(MyScene* sceneAtWork)
{
    myscene = sceneAtWork;

    //这个留下来，不出问题就不改，因为scene自己是设置为图片大小的，所以和图片一样应该

}

ItemRecognizer::ItemRecognizer(MyTextScene* sceneAtWork)
{
    //可能这里也需要删除
    textscene = sceneAtWork;

}

//测试看看有没有效果
ItemRecognizer::~ItemRecognizer()
{
    myscene->views().first()->update();
}

//这个函数原本用来根据item生成node的，现在升级了，以前只能识别line和word现在能识别image和table了
//但是函数里面啥也没变，变得是他调用的函数
void ItemRecognizer::constructNodefromPicItem()
{
    //点了槽函数才执行这个，才会有用，放在构造函数里面啥用也没有
    changeMyItems();

    rootNode = new HocrNode;
    rootNode->Clear();
    mainBodyNode = new HocrNode;
    mainBodyNode->Clear();
    rootNode->SetNodeClass("ocr_page");
    rootNode->SetNodeId("page_1");

    rootNode->SetBbox(0,0,mImage->width(),mImage->height());
    rootNode->SetPhysicPageNo("ppageno 0");
    //创造主体node
    makeMainBodyNode();
    //创造 行node 和 字node
    if(myscene->getBoardtype() == EDITBOARD) {
        makeLineNodeAndWordNode();
    }
    if(myscene->getBoardtype() == PROOFBOARD) {
        makeSortedLineNodeAndWordNode();   //这里其实是拍了序的，但是后面聚类校对一插字，顺序就会又变乱了?????
    }
}


void ItemRecognizer::makeMainBodyNode()
{
    for(MyItem* item : myItems) {
        if (item->nodeMode == MAIN_BODY) {
            rootNode->Clear();
            rootNode->AddChild(mainBodyNode);
            mainBodyNode->SetParent(rootNode);
            mainBodyNode->SetNodeClass("ocr_carea");
            mainBodyNode->SetNodeId("block_1_1");
            //item->setItemAxis();   前面统一设置了，所以这里取消
            //判断是否主体在scene的外面
            if(item->getBbox()->left < 0){
                item->bbox->left = 0;
            }
            if(item->getBbox()->top < 0){
                item->bbox->top = 0;
            }
            //假如还不行的话，那就把四个角都判断


            mainBodyNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
            qDebug() << "item->getBbox()->left" << item->getBbox()->left;
        }
    }
}

/**
 * @brief ItemRecognizer::makeSortedLineNodeAndWordNode   把item转化为node
 */
void ItemRecognizer::makeSortedLineNodeAndWordNode()

{
    QVector<MyItem*> all = sortingSingleRowItem();
    for(MyItem* item : all) {       //这个地方应该是排序后的return回来的东西
        if (item->nodeMode == SINGLE_ROW || item->nodeMode == DOUBLE_ROW) {
            mainBodyNode->AddChild(item->itemNode);
            item->itemNode->SetParent(mainBodyNode);
            item->itemNode->SetNodeClass("ocr_line");
            if(item->nodeMode == SINGLE_ROW) {
                item->itemNode->SetXFont("font_single_row");
            } else if (item->nodeMode == DOUBLE_ROW) {
                item->itemNode->SetXFont("font_double_row");
            }
            //清除是为了删除行下面的字
            item->itemNode->Clear();
            //item->setItemAxis();
            item->itemNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
            //qDebug() << "++++++++++++++++++++++++++++" << QString::fromStdString(item->itemNode->GetNodeClass());
            for(MyItem* i : myItems) {      //这个地方还是全的，要不不OK！！
                if(i->nodeMode == BIG_CHAR || i->nodeMode == SMALL_CHAR) {
                    //QPointF point = QPointF(i->rect().x() + 10 ,i->rect().y() + 10);  //这里的意思是取个中心点
                    QPointF point = QPointF(i->rect().x() + 5 ,i->rect().y() + 5);  //这里的意思是取个中心点
                    if(item->contains(point)) {
                        myCharItems.append(i);
                    }
                }
            }
            QVector<MyItem*> CharItems = sortingCharItem();
            for(MyItem* ii : CharItems) {
                ii->itemNode->SetParent(item->itemNode);
                item->itemNode->AddChild(ii->itemNode);
                ii->itemNode->SetNodeClass("ocrx_word");
                // ii->setItemAxis();
                ii->itemNode->SetBbox(ii->getBbox()->left,ii->getBbox()->top,ii->getBbox()->right,ii->getBbox()->bottom);
                ii->itemNode->SetXWconf(0);
                //qDebug()<< "left:   " <<ii->getBbox()->left <<"  top:  " <<ii->getBbox()->top << "   right:  " << ii->getBbox()->right;
            }
            myCharItems.clear();
        }  else if(item->nodeMode == TABLE) {
            mainBodyNode->AddChild(item->itemNode);
            item->itemNode->SetParent(mainBodyNode);
            item->itemNode->SetNodeClass("ocr_table");
            //item->setItemAxis();
            item->itemNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
        } else if(item->nodeMode == IMAGE) {
            mainBodyNode->AddChild(item->itemNode);
            item->itemNode->SetParent(mainBodyNode);
            item->itemNode->SetNodeClass("ocr_image");
            //item->setItemAxis();
            item->itemNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
        }
    }
}



/**
 * @brief ItemRecognizer::makeLineNodeAndWordNode   把item转化为node
 */
void ItemRecognizer::makeLineNodeAndWordNode()
{
    for(MyItem* item : myItems) {
        if (item->nodeMode == SINGLE_ROW) {
            mainBodyNode->AddChild(item->itemNode);
            item->itemNode->SetParent(mainBodyNode);
            item->itemNode->SetNodeClass("ocr_line");
            item->itemNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
            item->itemNode->SetXFont("font_single_row");
            qDebug() << "item->getBbox()->left" << item->getBbox()->left;
            //qDebug() << "++++++++++++++++++++++++++++" << QString::fromStdString(item->itemNode->GetNodeClass());
            for(MyItem* i : myItems) {
                if(i->nodeMode == BIG_CHAR) {
                    //QPointF point = QPointF(i->rect().x() + 10,i->rect().y() + 10);   //这个意思就是，只有在item里面的才归为那一个点
                    QPointF point = QPointF(i->rect().x() + 5,i->rect().y() + 5);   //这个意思就是，只有在item里面的才归为那一个点
                    if(item->contains(point)) {
                        i->itemNode->SetParent(item->itemNode);
                        item->itemNode->AddChild(i->itemNode);
                        i->itemNode->SetNodeClass("ocrx_word");
                        //i->setItemAxis();
                        i->itemNode->SetBbox(i->getBbox()->left,i->getBbox()->top,i->getBbox()->right,i->getBbox()->bottom);
                    }
                }
            }
        } else if(item->nodeMode == TABLE) {
            mainBodyNode->AddChild(item->itemNode);
            item->itemNode->SetParent(mainBodyNode);
            item->itemNode->SetNodeClass("ocr_table");
            //item->setItemAxis();
            item->itemNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
        } else if(item->nodeMode == IMAGE) {
            mainBodyNode->AddChild(item->itemNode);
            item->itemNode->SetParent(mainBodyNode);
            item->itemNode->SetNodeClass("ocr_image");
            //item->setItemAxis();
            item->itemNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
        } else if(item->nodeMode == DOUBLE_ROW) {
            mainBodyNode->AddChild(item->itemNode);
            item->itemNode->SetParent(mainBodyNode);
            item->itemNode->SetNodeClass("ocr_line");
            item->itemNode->SetBbox(item->getBbox()->left,item->getBbox()->top,item->getBbox()->right,item->getBbox()->bottom);
            item->itemNode->SetXFont("font_double_row");
            qDebug() << "item->getBbox()->left" << item->getBbox()->left;
            //qDebug() << "++++++++++++++++++++++++++++" << QString::fromStdString(item->itemNode->GetNodeClass());
            for(MyItem* i : myItems) {
                if(i->nodeMode == SMALL_CHAR) {
                    //QPointF point = QPointF(i->rect().x() + 10,i->rect().y() + 10);   //这个意思就是，只有在item里面的才归为那一个点
                    QPointF point = QPointF(i->rect().x() + 5,i->rect().y() + 5);   //这个意思就是，只有在item里面的才归为那一个点
                    if(item->contains(point)) {
                        i->itemNode->SetParent(item->itemNode);
                        item->itemNode->AddChild(i->itemNode);
                        i->itemNode->SetNodeClass("ocrx_word");
                        //i->setItemAxis();
                        i->itemNode->SetBbox(i->getBbox()->left,i->getBbox()->top,i->getBbox()->right,i->getBbox()->bottom);
                        i->itemNode->SetXFont("font_double_row");
                    }
                }
            }
        }
    }
}

void ItemRecognizer::constructHocrfromTextItemsNode()
{
    parser = new HocrParser;
    if(parser->Parse(pageNode)) {
        hocr = QString::fromStdString(parser->GetHocrString());
    }
}

void ItemRecognizer::constructHocrfromTextItemsNode(HocrNode* pageNode)
{
    parser = new HocrParser;
    if(parser->Parse(pageNode)) {
        hocr = QString::fromStdString(parser->GetHocrString());
    }
}


void ItemRecognizer::setImage(QImage *image)
{
    mImage = image;
}


QString ItemRecognizer::constructHocrfromNode(HocrNode* rootNode)
{
    parser = new HocrParser;
    if(parser->Parse(rootNode)) {
        string hocr = parser->GetHocrString();
        QString str = QString::fromStdString(hocr);
        qDebug()<<str;
        str = str.replace('""','\"');
        str = str.replace("\n","\\n").toHtmlEscaped();
        recognizerSendHocr(str);
        return str;
    } else {
        return "you have a error";
    }
}

/**
 * @brief ItemRecognizer::constructHocrfromNode  我要弄一个overload 因为换函数用了，也就是说这个函数已经没用了
 * 先留着以后看看，这个我就不优化了
 * @return
 */
QString ItemRecognizer::constructHocrfromNode()
{

    //qDebug()<<myscene->items().size();
    if(myscene->items().size() == 0){
        QMessageBox messagebox;
        messagebox.information(NULL,QStringLiteral("提示"),QStringLiteral("未检测到操作，上传失败"), QMessageBox::Yes);
        return "itemreconizer error";
    }
    XMLDocument doc;
    doc.Clear();
    //插入最外层
    XMLElement* root = doc.NewElement("div");
    root->SetAttribute("class",rootNode->GetNodeClass().c_str());
    root->SetAttribute("id",rootNode->GetNodeId().c_str());

    std::string rootNodeBbox = "bbox 0 0 " + std::to_string(mImage->width()) + " " + std::to_string(mImage->height());
    root->SetAttribute("title",rootNodeBbox.c_str());
    doc.InsertEndChild(root);
    //插入主体框
    XMLElement* mainBody = doc.NewElement("div");
    mainBody->SetAttribute("class",mainBodyNode->GetNodeClass().c_str());
    mainBody->SetAttribute("id",mainBodyNode->GetNodeId().c_str());
    std::string mainBodyBbox = "bbox " + std::to_string(mainBodyNode->GetBbox().left) +  " " + std::to_string(mainBodyNode->GetBbox().top) +  " " + std::to_string(mainBodyNode->GetBbox().right) + " " +  std::to_string(mainBodyNode->GetBbox().bottom);
    mainBody->SetAttribute("title",mainBodyBbox.c_str());
    root->InsertEndChild(mainBody);
    //插入竖行
    for(int i = 0; i < mainBodyNode->GetChildren().size(); i++) {
        XMLElement* linenode = doc.NewElement("span");
        linenode->SetAttribute("class","ocr_line");
        //算出linenodeBbox 的 string
        HocrNode* node = mainBodyNode->GetChildren().at(i);
        std::string linenodeBbox = "bbox " + std::to_string(node->GetBbox().left) + " " + std::to_string(node->GetBbox().top) + " " + std::to_string(node->GetBbox().right) + " " +  std::to_string(node->GetBbox().bottom);
        linenode->SetAttribute("title",linenodeBbox.c_str());
        mainBody->InsertEndChild(linenode);
        for(int i = 0;i < node->GetChildren().size(); i++) {
            //插入单字
            XMLElement* word = doc.NewElement("span");
            word->SetAttribute("class","ocrx_word");
            HocrNode* nd = node->GetChildren().at(i);
            std::string wordBbox = "bbox " + std::to_string(nd->GetBbox().left) + " " + std::to_string(nd->GetBbox().top) + " " + std::to_string(nd->GetBbox().right) + " " +  std::to_string(nd->GetBbox().bottom);
            word->SetAttribute("title",wordBbox.c_str());
            word->InsertEndChild(doc.NewText(""));
            linenode->InsertEndChild(word);
        }
    }
    XMLPrinter printer;
    printer.ClearBuffer();
    doc.Print(&printer);
    QString str = QString(printer.CStr()).replace('""','\"');

    str = str.replace("\n","\\n").toHtmlEscaped();

    recognizerSendHocr(str);
    return str;
}

void ItemRecognizer::setMyscene(MyScene *value)
{
    myscene = value;
}

// 把所有item都转化了，省得到里面在转换
void ItemRecognizer::changeMyItems()
{
    //史上最贵的一个clear
    myItems.clear();
    myCharItems.clear();
    for(int i = 0;i< myscene->items().size();i++){
        myitem = (MyItem*)(myscene->items().at(i));
        myitem->setItemAxis();
        myItems.append(myitem);
    }
}


void ItemRecognizer::makePicItemfromHocr(QString hocr)
{
    //qDebug()<< hocr;
    //清空之前的item信息
    parser = new HocrParser;
    qDebug()<<hocr;
    if (parser->Parse(hocr.toStdString().c_str())) {
        HocrNode* pageNode = parser->GetPageNode();
        HocrNode* mainBodyNode = pageNode->GetChildren().at(0);
        MyItem* MainBodyItem = new MyItem;
        myscene->setPenColorAndMode(MAIN_BODY);
        MainBodyItem->setNodeMode(MAIN_BODY);
        MainBodyItem->setPen(myscene->pen);
        MainBodyItem->setRect(mainBodyNode->GetBbox().left , mainBodyNode->GetBbox().top,
                              mainBodyNode->GetBbox().right - mainBodyNode->GetBbox().left,
                              mainBodyNode->GetBbox().bottom - mainBodyNode->GetBbox().top);
        myscene->addItem(MainBodyItem);
        //MainBodyItem->setFlag(QGraphicsItem::ItemIsSelectable,false);

        for(HocrNode* linenode:mainBodyNode->GetChildren()) {
            MyItem* lineNodeItem = new MyItem;
            //在等于line里面加一个判断
            if(QString::fromStdString(linenode->GetNodeClass()) == "ocr_line"){
                //内存if
                if(QString::fromStdString(linenode->GetXFont()) == "font_single_row"){
                    myscene->setPenColorAndMode(SINGLE_ROW);
                    lineNodeItem->setNodeMode(SINGLE_ROW);
                    lineNodeItem->setPen(myscene->pen);
                    lineNodeItem->setRect(linenode->GetBbox().left , linenode->GetBbox().top,
                                          linenode->GetBbox().right - linenode->GetBbox().left,
                                          linenode->GetBbox().bottom - linenode->GetBbox().top);
                    lineNodeItem->setBoardtype(PROOFBOARD);
                    myscene->addItem(lineNodeItem);
                    if((linenode->GetChildren()).size() == 0)
                        continue;
                    for(HocrNode* charnode:linenode->GetChildren()) {
                        //避免第一步出现大字item
                        if(myscene->getBoardtype() ==  EDITBOARD) {
                            break;
                        }
                        MyItem* charItem = new MyItem;
                        myscene->setPenColorAndMode(BIG_CHAR);
                        charItem->setNodeMode(BIG_CHAR);
                        charItem->setPen(myscene->pen);
                        charItem->setRect(charnode->GetBbox().left , charnode->GetBbox().top,
                                          charnode->GetBbox().right - charnode->GetBbox().left,
                                          charnode->GetBbox().bottom - charnode->GetBbox().top);
                        myscene->addItem(charItem);
                    }
                } else  if(QString::fromStdString(linenode->GetXFont()) == "font_double_row") { //内层else
                    myscene->setPenColorAndMode(DOUBLE_ROW);
                    lineNodeItem->setNodeMode(DOUBLE_ROW);
                    lineNodeItem->setPen(myscene->pen);
                    lineNodeItem->setRect(linenode->GetBbox().left , linenode->GetBbox().top,
                                          linenode->GetBbox().right - linenode->GetBbox().left,
                                          linenode->GetBbox().bottom - linenode->GetBbox().top);
                    lineNodeItem->setBoardtype(PROOFBOARD);
                    myscene->addItem(lineNodeItem);
                    if((linenode->GetChildren()).size() == 0)
                        continue;
                    for(HocrNode* charnode:linenode->GetChildren()) {
                        //避免第一步出现大字item
                        if(myscene->getBoardtype() ==  EDITBOARD) {
                            break;
                        }
                        MyItem* charItem = new MyItem;
                        myscene->setPenColorAndMode(SMALL_CHAR);
                        charItem->setNodeMode(SMALL_CHAR);
                        charItem->setPen(myscene->pen);
                        charItem->setRect(charnode->GetBbox().left , charnode->GetBbox().top,
                                          charnode->GetBbox().right - charnode->GetBbox().left,
                                          charnode->GetBbox().bottom - charnode->GetBbox().top);
                        myscene->addItem(charItem);
                    }
                }
            } else if(QString::fromStdString(linenode->GetNodeClass()) == "ocr_image"){
                myscene->setPenColorAndMode(IMAGE);
                lineNodeItem->setNodeMode(IMAGE);
                lineNodeItem->setPen(myscene->pen);
                lineNodeItem->setRect(linenode->GetBbox().left , linenode->GetBbox().top,
                                      linenode->GetBbox().right - linenode->GetBbox().left,
                                      linenode->GetBbox().bottom - linenode->GetBbox().top);
                lineNodeItem->setBoardtype(PROOFBOARD);
                myscene->addItem(lineNodeItem);
            } else if(QString::fromStdString(linenode->GetNodeClass()) == "ocr_table"){
                myscene->setPenColorAndMode(TABLE);
                lineNodeItem->setNodeMode(TABLE);
                lineNodeItem->setPen(myscene->pen);
                lineNodeItem->setRect(linenode->GetBbox().left , linenode->GetBbox().top,
                                      linenode->GetBbox().right - linenode->GetBbox().left,
                                      linenode->GetBbox().bottom - linenode->GetBbox().top);
                lineNodeItem->setBoardtype(PROOFBOARD);
                myscene->addItem(lineNodeItem);
            }
        }
    }
}

/**
 * @brief ItemRecognizer::makeTextItemfromHocr  改字显示的地方，这个模块用到了整页校对的显字上
 * @param hocr
 */
void ItemRecognizer::makeTextItemfromHocr(QString hocr)     // 改字的显示的地方
{
    parser = new HocrParser;
    if (parser->Parse(hocr.toStdString().c_str())) {
        pageNode = parser->GetPageNode();
        HocrNode* mainBodyNode = pageNode->GetChildren().at(0);
        //字体图像显示
        int _left = 10000;
        int _top = 10000;
        for (HocrNode *line : mainBodyNode->GetChildren()) {
            for (HocrNode *word : line->GetChildren()) {
                if (word->GetBbox().left < _left)
                    _left = word->GetBbox().left;

                if (word->GetBbox().top < _top)
                    _top = word->GetBbox().top;
            }
        }

        for(HocrNode* linenode : mainBodyNode->GetChildren()) {

            if((linenode->GetChildren()).size() == 0)
                continue;

            for(HocrNode* charnode:linenode->GetChildren()) {
                MyTextItem* charItem = new MyTextItem;

                charItem->setBoardType(FULLPICPROOF);

                textscene->setSceneRect(0,0,1920/9*3,900);

                charItem->setPlainText(QString(charnode->GetCharacter().c_str()));
                /*************这个是把整页校对没认出来的字打成@的方法*******************/
                if(QString(charnode->GetCharacter().c_str()) == ""){
                    charItem->setPlainText("@");
                }
                /****************************************************************/
                //                charItem->setDefaultTextColor(Qt::red);
                //设置字的大小
                //charItem->setItemSize(40);
                charItem->setItemSize(30);
                //                charItem->x = charnode->GetBbox().left;
                //                charItem->y = charnode->GetBbox().top;
                charItem->w = charnode->GetBbox().right - charnode->GetBbox().left;
                charItem->h = charnode->GetBbox().bottom - charnode->GetBbox().top;
                //调节整页校对坐标的地方
                //charItem->setPos((charnode->GetBbox().left - _left) + 10 ,(charnode->GetBbox().top - _top) + 10 );
                charItem->setPos((((charnode->GetBbox().left - _left) + 10)*0.7) ,(((charnode->GetBbox().top - _top) + 10 ))*0.7);
                //？？
                charItem->x = 10 - _left ;
                charItem->y = 10 - _top ;
                textscene->addItem(charItem);
                //设置item伴随node
                charItem->setItemNode(charnode);

                textscene->words.append(charItem);
            }
        }
    }

    for(int i = 0; i < textscene->items().size(); i++){
        textscene->items().at(i)->setFlag(QGraphicsTextItem::ItemIsFocusable);
    }
    //textscene->
}



/**
 * @brief ItemRecognizer::returnHocrNode  整页校对 把Node return 回去
 * @param column
 */
HocrNode* ItemRecognizer::returnHocrNode(QString hocr)
{
    parser = new HocrParser;
    if (parser->Parse(hocr.toStdString().c_str())) {
        pageNode = parser->GetPageNode();
        return pageNode;
    }
}


QString ItemRecognizer::getHocr() const
{
    return hocr;
}

HocrNode* ItemRecognizer::changeHocrToPageNode(QString hocr)
{
    HocrParser* parser = new HocrParser;
    HocrNode* pageNode;
    if (parser->Parse(hocr.toStdString().c_str())) {
        pageNode = parser->GetPageNode();
    } else{
        //qDebug()<<"your clusterproof has an error,changeHocrToPageNode doesn't work ";
    }
    return pageNode;
}

/**
 * @brief ItemRecognizer::fromNodeFillInMaps  这个函数是聚类校对时候，切割单个图片然后传入map的
 * @param image
 * @param node
 * @param map
 * @param index
 */
void
ItemRecognizer::fromNodeFillInMaps(QImage image,
                                   HocrNode* node,
                                   QVector<CharAndCharRegionList*> *listVector,
                                   int index)
{
    HocrNode* mainBodyNode = node->GetChildren().at(0);
    for(HocrNode* lineNode : mainBodyNode->GetChildren()) {
        for(HocrNode* wordNode : lineNode->GetChildren()) {
            //下面这个发生了一个bug，记住，要在恰当的时候new ，早了也非常可怕， 早了晚了都很可怕
            CharPicInfo* picInfo = new CharPicInfo;
            picInfo->index = index;
            picInfo->bbox = wordNode->GetBbox();
            //qDebug()<<picInfo->bbox.left<<"  "<<picInfo->bbox.top<<"  "<<picInfo->bbox.right - picInfo->bbox.left<< "   "<< picInfo->bbox.bottom - picInfo->bbox.top;
            picInfo->image = image.copy(picInfo->bbox.left,
                                        picInfo->bbox.top,
                                        picInfo->bbox.right - picInfo->bbox.left,
                                        picInfo->bbox.bottom - picInfo->bbox.top);
            //qDebug()<<picInfo->image.byteCount();

            if(isExist(listVector, QString::fromStdString(wordNode->GetCharacter()))) {
                for(CharAndCharRegionList *list : *listVector){
                    //纠正了聚类校对的bug
                    //测试，多写注释，多写注释，这里是测试两个字是不是相等的，因为出现多字了，所以我们这里取第一个值
                    if(list->character.left(1) == (QString::fromStdString(wordNode->GetCharacter())).left(1) ) {
                        //QString q = list->character.left(0);
                        list->regionList.append(picInfo);
                    }
                }
            } else {
                CharAndCharRegionList* list = new CharAndCharRegionList;
                list->character = QString::fromStdString(wordNode->GetCharacter());
                list->regionList.append(picInfo);
                listVector->append(list);
            }

        }
        for(int i = 0;i< listVector->size();i++) {
            for(int j = 0; j<listVector->at(i)->regionList.size();j++){
                //qDebug()<< listVector->at(i)->regionList.at(j)->image.byteCount();
            }
        }
    }
}

/**
 * @brief ItemRecognizer::isExist 测试vector内某个字是否存在
 * @param listVector
 * @return
 */
bool ItemRecognizer::isExist(QVector<CharAndCharRegionList*>* listVector, QString str)
{
    //    qDebug()<<listVector->at(0)->character;
    //    if(listVector->size() == 0) {
    //        return false;
    //    }
    for(CharAndCharRegionList* list : * listVector) {
        if(list->character.left(1) == str.left(1))
            return true;

    }
    return false;   //这里可能有些错误
}

/**
 * @brief ItemRecognizer::sortingSingleItem   给Item排个序，避免来回乱蹦的情景！！！！！！ 目前还有将来好像都是仅在版面校对这里使用
 */
QVector<MyItem*> ItemRecognizer::sortingSingleRowItem(){
    xsForSortItemForNodeRange.clear();
    for(MyItem* item : myItems){
        if(item->nodeMode == SINGLE_ROW ||item->nodeMode == TABLE ||item->nodeMode == IMAGE ||item->nodeMode == DOUBLE_ROW)
            //加入这个if判断的原因是，假如单行和双行小注，或者两个单行的left一致的话，
            //那么就会出现两份这两个，重复的越多，越麻烦，排序在这里就会出这样的问题
            if(!xsForSortItemForNodeRange.contains(item->getBbox()->left)) {
                xsForSortItemForNodeRange.append(item->getBbox()->left);
            }
    }
    qSort(xsForSortItemForNodeRange.begin(),xsForSortItemForNodeRange.end(),qGreater<int>());
    QVector<MyItem*> items;
    for(int i:xsForSortItemForNodeRange){   //i is for int
        for(MyItem* item : myItems){
            if((i == item->getBbox()->left) && (item->nodeMode == SINGLE_ROW ||item->nodeMode == TABLE ||item->nodeMode == IMAGE
                                                ||item->nodeMode == DOUBLE_ROW))
                items.append((MyItem*)item);
        }
    }
    return items;
}

/**
 * @brief ItemRecognizer::sortingCharItem   直接在这里面调用 class instance了  不知道是好是坏
 * @return
 */
QVector<MyItem*> ItemRecognizer::sortingCharItem(){
    // 加上下面这句看看好使不
    xsForSortItemForNodeRange.clear();
    for(MyItem* i:myCharItems) {
        //加入这个if判断的原因是，假如加入误画的框子，导致错误的顶相等的大字重复
        if(!xsForSortItemForNodeRange.contains(i->getBbox()->top)) {
            xsForSortItemForNodeRange.append(i->getBbox()->top);
        }
    }
    qSort(xsForSortItemForNodeRange.begin(),xsForSortItemForNodeRange.end());
    QVector<MyItem*> items;
    for(auto i:xsForSortItemForNodeRange){   //i is for int
        for(MyItem* item : myCharItems){
            if(i == item->getBbox()->top && ((item->nodeMode == BIG_CHAR) || (item->nodeMode == SMALL_CHAR)))
                items.append((MyItem*)item);
        }
    }
    xsForSortItemForNodeRange.clear();
    myCharItems.clear();
    return items;
}

/**
 * @brief ItemRecognizer::sortFullPicNode  这个是整页校对里面一开始给Node排序的
 */
void ItemRecognizer::sortFullPicNode()
{
    parser = new HocrParser;
    if (parser->Parse(hocr.toStdString().c_str())) {
        pageNode = parser->GetPageNode();
        HocrNode* mainBodyNode = pageNode->GetChildren().at(0);
        //字体图像显示

        for(HocrNode* linenode : mainBodyNode->GetChildren()) {

            if((linenode->GetChildren()).size() == 0)
                continue;

            for(HocrNode* charnode:linenode->GetChildren()) {

            }
        }
    }
}



