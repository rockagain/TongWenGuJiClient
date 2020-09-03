#include "fullpicproof.h"
#include "ui_fullpicproof.h"
using namespace tinyxml2;

FullPicProof::FullPicProof(QString projectid, QWidget *parent, QString username) :
    QWidget(parent),
    ui(new Ui::FullPicProof)
{
    ui->setupUi(this);
    this->userName = username;
    //projectId 赋值
    projectId = projectid;
    //整体申请时候的数
    splitter = new QSplitter();

    mView = new MyView;
    mScene = new MyScene;
    mView->setScene(mScene);
    mView->setStyleSheet("background-color:#eeeeee;");
    mView->setBoardType(FULLPICPROOF);
    textView = new MyView;
    //textView->scale(0.628,0.628);
    textScene = new MyTextScene;
    textScene->setBoardType(FULLPICPROOF);
    textView->setScene(textScene);
    textView->setBoardType(FULLPICPROOF);
    // mView->setSceneRect(0,0,1920/9*4,900);
    //textView->setSceneRect(0, 0, 1920/9*3, 900);
    myListWidget = new MyListWidget;
    myListWidget->setFixedWidth(300);
    splitter->addWidget(myListWidget);
    splitter->addWidget(mView);
    splitter->addWidget(textView);
    splitter->setStretchFactor(0,2);
    splitter->setStretchFactor(1,4);
    splitter->setStretchFactor(2,3);

    ui->verticalLayout->addWidget(splitter,Qt::AlignCenter);
    ColumnPic = new MyPixmapItem;

    netRequest = new NetWorkClub(this->userName);
    netDownLoad = new NetWorkClub(this->userName);
    picUpload = new NetWorkClub(this->userName);
    netDownLoad->NetRequestType = PicDownload;
    itemrecognizer = new ItemRecognizer(textScene);
    dialogOnOrNot = true;

    picDbRequest();
    initRatio();
    onlyPicture = true;

    connect(netRequest, &NetWorkClub::setWholePicRequestInfo,this, &FullPicProof::receiveWholePicRequestInfo);
    connect(myListWidget, &MyListWidget::itemDoubleClicked,this,&FullPicProof::downLoadPic);
    connect(netDownLoad, &NetWorkClub::sentPicInfo,this,&FullPicProof::receivePicInfo);
    //  connect(mScene,&MyScene::sceneSendHocr,this,&FullPicProof::receiveHocrToGo);
    connect(textScene,&MyTextScene::focusItemChanged,mScene,&MyScene::focusItemChangedResponse);
    //上面这个玩意出问题了，到时候搞一搞
    connect(picUpload,&NetWorkClub::uploadFinishedSignal,this,&FullPicProof::changeProgressBarSlots);
    connect(textScene,&MyTextScene::activeTheSingleLineFuncion,this,&FullPicProof::showSingleColumn);
}

FullPicProof::~FullPicProof()
{
    delete ui;
}

void FullPicProof::picDbRequest()
{
    database = new MyDatabase("./wholePicProof.db");
    //下面写读取的东西  的过程， 意思就是 query 一下  空就申请，不空就读取里面的东西，然后显示

    if(!database->isDataBaseEmptyOrNot()){
        qDebug()<<database->query->value(0);
        //这个方法是根据查询sqlite得到结果后，生成书得信息得，这里得封装得方法和  申请那里不同，那里没有搞
        constructPic();
        while(database->query->next()) {
            qDebug()<< database->query->value(0);
            constructPic();
        }
        for(int i = 0 ;i<myListWidget->count();i ++) {
            myListWidget->setCurrentRow(i);
            if (i % 2 == 0){
                myListWidget->currentItem()->setBackground(QBrush(QColor("#eaeaea")));
            }
        }
    }
}

//查询数据可，生成书的信息
void FullPicProof::constructPic()
{
    pic = new PicEntityAndInfo;
    pic->fileCode = database->query->value(0).toString();
    pic->fullName = database->query->value(1).toString();
    pic->fileName = database->query->value(2).toString();
    pic->filePath = database->query->value(3).toString();
    pic->isFinishedOrNot = database->query->value(4).toBool();
    pic->fileOcr = database->query->value(5).toString();
    pics.append(pic);

    //偷个懒在这里直接把生成的书的数据上传给左侧栏上了
    constructPicItem(pic->fileName,pic->isFinishedOrNot);
}

void FullPicProof::constructPicItem(QString filename,bool isFinishedOrNot)
{
    QListWidgetItem* item = new QListWidgetItem;
    //一开始调整颜色的地方
    if(isFinishedOrNot) {
        item->setTextColor(Qt::gray);
    } else {
        item->setTextColor(Qt::black);
    }

    item->setText(filename);
    myListWidget->addItem(item);
}

/**
 * @brief 申请  图片信息
 */
void FullPicProof::on_requestButton_clicked()
{
    

    if(database->isDataBaseEmptyOrNot()) {
        textScene->clear();
        totalNumber = requestNumber;
        dialogOnOrNot = true;
        QString num = QString::number(totalNumber,10);
        pics.clear();
        //申请图片数量自己调
        netRequest->NetRequestType = WholePicRequest;
        //&
        netRequest->wholePicRequest(projectId,this->userName,num);
    }
    //    else if(testToSeeIfEeveryPicIsFinished()) {          //这里的意思就是把弄完的都删除，然后申请新的
    //        pics.clear();
    //        myListWidget->clear();
    //        //mScene->clear();
    //        database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '1'");
    //        netRequest->NetRequestType = WholePicRequest;
    //        netRequest->wholePicRequest(projectId,this->userName,num);
    //        //这里的意思是新换一批的时候，把所有的item删除
    //        for (QGraphicsItem* item : mScene->items()) {
    //            mScene->removeItem(item);
    //        }
    //    }
}

// 轮查图片的状态
bool FullPicProof::testToSeeIfEeveryPicIsFinished()
{
    for(PicEntityAndInfo* p: pics) {
        if (!p->isFinishedOrNot) {
            return false;
        }
    }
    return true;
}

void FullPicProof::receiveWholePicRequestInfo(QString info)
{
    QJsonObject obj = netRequest->getJsonObjectFromString(info);
    QJsonValueRef  ref = obj["fileList"];
    QJsonArray arrary =  ref.toArray();
    for(int i = 0; i < arrary.size() ; i++) {
        //分析 返回的 info
        QJsonValueRef rf = arrary[i];
        QJsonObject ob =  rf.toObject();
        QString fullname = ob["fileName"].toString();
        QListWidgetItem* item = new QListWidgetItem;
        QFileInfo mFile(fullname);
        QString filename;
        filename = mFile.fileName().replace(mFile.suffix(),"");
        filename = filename.left(filename.length() - 1);
        item->setTextColor(Qt::black);
        item->setText(filename);
        if (i % 2 == 0)
            item->setBackground(QBrush(QColor("#eaeaea")));
        myListWidget->addItem(item);
        //填充book信息
        pic = new PicEntityAndInfo;

        // pic->id = ob["id"].toString();
        pic->fileCode = QString::number(ob["fileId"].toInt(),10);
        pic->fullName = fullname;
        pic->fileName = filename;
        pic->filePath = ob["path"].toString();
        pic->isFinishedOrNot = false;
        pic->fileOcr = ob["hocrText"].toString();
        pics.append(pic);

        //把数据都存到 数据库内
        QString exc_str = QString("INSERT INTO tb_book VALUES('%1','%2','%3','%4','%5','%6')").arg(pic->fileCode).arg(pic->fullName).arg(pic->fileName).arg(pic->filePath).arg(pic->isFinishedOrNot).arg(pic->fileOcr);
        qDebug()<<database->query->exec(exc_str);
    }
    //下载完了以后显示提示框
    QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("申请成功"));
    messagebox.setStandardButtons(QMessageBox::Ok);
    messagebox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
    //messagebox.setButtonText(QMessageBox::Cancel,QStringLiteral("下一张"));
    int reply = messagebox.exec();
    //qDebug()<<reply;

    if(reply == 1024){
        downLoadtheFirstPic();
    }
}

//download 下载图片
void FullPicProof::downLoadPic(QListWidgetItem *item)
{
    netDownLoad->NetRequestType = PicDownload;
    for(PicEntityAndInfo* i :pics) {
        if(i->fileName == item->text()) {
            downloadPicName = i->filePath  ;   //+ "//" + i->fullName;  //服务器改了
            downloadPicName.replace("\\","//");
            //qDebug()<<downloadPicName<<"12121212&&&&&"<<"12121@@@";
            currentPic = i;
            break;
        }
    }
    //qDebug()<<downloadPicName;
    textScene->clear();
    netDownLoad->downLoad(this->userName,downloadPicName);
    textScene->setSceneRect(0, 0, textView->width(), textView->height());
    this->downloadOksignal();
}

/**
 * @brief FullPicProof::downLoadtheFirstPic   只有在申请的时候才会自动下下载第一张图片
 */
void FullPicProof::downLoadtheFirstPic()
{
    PicEntityAndInfo* i =pics.at(0);
    downloadPicName = i->filePath;
    downloadPicName.replace("\\","//");
    currentPic = i;
    netDownLoad->downLoad(this->userName,downloadPicName);
    myListWidget->setCurrentRow(0);
    this->downloadOksignal();
}

/**
 * @brief 信号槽返回的，获得图片内容信息的,显示图片
 * @param all
 */
void FullPicProof::receivePicInfo(QString all)
{
    textScene->words.clear();
    //qDebug()<<all;

    //这里的意思是接收新图片的时候，把所有的item删除
    for (QGraphicsItem* item : mScene->items()) {
        mScene->removeItem(item);
    }

    QJsonObject obj = netRequest->getJsonObjectFromString(all);
    QJsonValueRef  ref = obj["fileInfo"];

    QString pic =  ref.toString();
    QByteArray bytearray;
    bytearray += pic;
    QByteArray picArray;
    picArray = picArray.fromBase64(bytearray);
    image = new QImage;
    image->loadFromData(picArray);

    QPainter* mPainter = new QPainter;
    QRectF mRectf;

    mScene->setImage(image);
    //mScene->setSceneRect(0, 0, image->width()*3/4, image->height()*3/4);
    mScene->setSceneRect(0, 0, image->width(), image->height());
    //textScene->setSceneRect(0,0,image->width(),image->height());  //标记一下
    mScene->drawBackground(mPainter,mRectf);
    //qDebug()<< "look for signal";
    mScene->update(0,0,image->width(),image->height());

    //下面这一段(if里面的)是处理一开始下载下来的图片的显示的，要 刚刚好
    //%%
    ratio = mScene->height()/mView->height();
    if(ratioSwitch){
        if(ratioIsChangedByResize){
            mView->scale(reSizeRatio,reSizeRatio);
            //这里把右边的字的scene也设置好是不是就可以了
            ratioIsChangedByResize = false;
        }
        mView->scale(1/ratio ,1/ratio );
        //把所有设置ratio的地方都加上textView
        ratioSwitch = false;
    }
    //写在这里为真是因为一上来就resize的话，那就啥也没有了
    resizeRatioSwitch = true;

    mView->update();
    textView->update();

    //把刚才的item保存下来，去之后那里把它拉灰，就可以了 ,接收图片后就赋值，这样就好一些
    //从这里是最科学的
    currentSeletedItem = new QListWidgetItem;
    currentSeletedItem =  myListWidget->currentItem();

    //把传回来的hocr 转换为  字的框框
    changeHocrToItemTheFirst();
}

//把传回来的hocr，转换为Item对象
void FullPicProof::changeHocrToItemTheFirst()
{
    //在这里初始化明显比较合理一些
    onlyPicture = true;

    if(currentPic->fileOcr == "")
        return;
    QString hocr = ProofBoard::convertTheHocrFormats(currentPic->fileOcr);

    PageNode =  itemrecognizer->returnHocrNode(hocr);

    MainBodyNode = PageNode->GetChildren().at(0);

    setWordNodeBeforePunc();

    sortedLineLeft.clear(); //这么做是每次新下载一张图片都清除一下

    textScene->proof_CurrentColumnIndex = 0;   //当前在那, 初始化参数了
    //假如这里没有字，那么就直接保存
    for(HocrNode* line:MainBodyNode->GetChildren()) {
        if(QString::fromStdString(line->GetNodeClass()) == "ocr_line"){
            onlyPicture = false;
        }
    }

    if(onlyPicture) {
        //假如真的只有图片的话，那么就直接上传
        this->okToGo();
        return;
    }
    lineNum = 0 ;
    for(HocrNode* line:MainBodyNode->GetChildren()) {
        if(line->GetChildren().size() == 0)
            continue;
        //这里只能算line的数量，其余的不显示啊
        if(QString::fromStdString(line->GetNodeClass()) == "ocr_line"
                && line->GetChildren().size() != 0) {
            lineNum ++;
        }
        sortedLineLeft.append(line->GetBbox().left);
    }
    textScene->proof_wholeColumnNum =  lineNum;  //当前一共多少行
    qSort(sortedLineLeft.begin(),sortedLineLeft.end(),qGreater<int>());
    //itemrecognizer->makeTextItemfromHocr(hocr);  //这个是之前的显示最右边字的函数
    showSingleColumn(0);
}

/**
 * @brief FullPicProof::showSingleColumn  显示每一单元啊， 然后切的时候是 上下的时候切到最后或者最前的时候切
 * @param Column
 */
void FullPicProof::showSingleColumn(int Column) {
    textScene->clear();
    for(HocrNode* line: MainBodyNode->GetChildren()) {
        //一开始用break  崩溃了，没想好，这里不就是需要用break么
        if(QString::fromStdString(line->GetNodeClass()) != "ocr_line")
            continue;
        if(sortedLineLeft.at(Column) == line->GetBbox().left) {
            //切图，然后，调用函数，给每一列排序，然后显示每一个字
            //textScene->setSceneRect(0,0,1920/9*3,1080);
            textScene->setSceneRect(textView->rect());
            //textScene->setSceneRect(0,0,textView->width(),textView->height());
            QPixmap pixmap;
            _image = image->copy(line->GetBbox().left,
                                 line->GetBbox().top,
                                 line->GetBbox().right - line->GetBbox().left,
                                 line->GetBbox().bottom - line->GetBbox().top);
            //qDebug() << _image.byteCount();
            ColumnPic = new MyPixmapItem;
            //qreal ratio =qreal(_image.height())/qreal(textView->height());
            ColumnPic->setPixmap(((pixmap.fromImage(_image))/*.scaled(ratio,ratio)*/));

            textView->setSceneRect(0,0,textView->width(),textView->height());

            textScene->addItem(ColumnPic);


            ColumnPic->setX(0);
            ColumnPic->setY(0);
            //缩整图片的大小
            ColumnPic->setScale(0.5);
            showSingleColumnText(line);
        }
    }
}

/**
 * @brief FullPicProof::showSingleColumnText   显示每一行配套的字
 */
void FullPicProof::showSingleColumnText(HocrNode* line)
{
    textScene->words.clear();

    for(HocrNode* word : line->GetChildren()) {
        sortedWordTop.append(word->GetBbox().top);
    }

    //这里可能需要分类，然后重新弄了，包括循环那里
    textScene->proof_WordNumPerColumn = line->GetChildren().size();    //每行多少字

    qSort(sortedWordTop.begin(),sortedWordTop.end());

    for(int top : sortedWordTop) {
        for(HocrNode* word : line->GetChildren()) {
            //if word 的内容不是 "，" "。" 那就把它们收入 words
            if((QString::fromStdString(word->GetCharacter()) != "，") &&
                    (QString::fromStdString(word->GetCharacter()) != "。") &&
                    (top == word->GetBbox().top)) {
                textItem = new MyTextItem;
                //判断这个是否有
                if(word->hasPuncNode) {
                    textItem->hasPuncItem = true;
                    textItem->puncNode = new HocrNode;
                    textItem->puncNode->SetCharacter(word->hasWhatPunc);
                }
                connect(textItem,&MyTextItem::textSceneAddItemSlot,
                        textScene,&MyTextScene::showPuncItem);
                //下面这个30  是和 textItem size 那里的30一样的   //这里是设置字的
                //textItem->setPos((_image.width())/2,(word->GetBbox().bottom - sortedWordTop.at(0) - 30)/2);
                textItem->setItemSize(20);
                textItem->setPos((_image.width())/2,(word->GetBbox().bottom - sortedWordTop.at(0) - 20)/2 - 5);
                textItem->setPlainText(QString(word->GetCharacter().c_str()));
                qDebug()<< textItem->toPlainText();
                //这里其实就是调节  "一"  而做的特殊判断
                if(textItem->toPlainText() == QStringLiteral("一") ){
                    textItem->setPos((_image.width())/2,(word->GetBbox().bottom - sortedWordTop.at(0) )/2 );
                }
                textItem->setFlag(QGraphicsTextItem::ItemIsFocusable);
                textItem->w = word->GetBbox().right - word->GetBbox().left;
                textItem->h = word->GetBbox().bottom - word->GetBbox().top;
                textItem->x = word->GetBbox().left ;
                textItem->y = word->GetBbox().top ;

                textItem->setBoardType(FULLPICPROOF);
                textScene->addItem(textItem);
                textItem->setItemNode(word);
                textScene->words.append(textItem);
            }
        }
    }
    sortedWordTop.clear();
    textScene->showPuncItem();
}

void FullPicProof::setRequestNumber(int value)
{
    requestNumber = value;
}

/***************************和绘画有关的，还有和图片放大缩小等按钮什么的***********************************************************/

//放大
void FullPicProof::on_zoomOutButton_clicked()
{
    mScene->setMode(ManageMode::ZOOMOUT);
    mScene->views().first()->scale(0.9,0.9);
}

/**
 * @brief 通过按钮设置模式
 */
void FullPicProof::on_zoomInButton_clicked()
{
    mScene->setMode(ManageMode::ZOOMIN);
    mScene->views().first()->scale(1.1,1.1);
}

/**
 * @brief 通过按钮设置模式
 */
void FullPicProof::on_editButton_clicked()
{
    mScene->setMode(ManageMode::EDIT);
    mView->setFocus();
}

/**
 * @brief 这个是Item的颜色，来区分遮盖的
 */
void FullPicProof::on_setColorButton_clicked()
{
    mScene->setColorToCheck();
}

/**
 * @brief combobox的index改变，然后触发这个函数，设置Item外沿color，还有其实里面把mode也改了
 * @param index
 */
void FullPicProof::on_comboBox_currentIndexChanged(int index)
{
    mScene->setPenColorAndMode(index);
    mView->setFocus();
}

/**
 * @brief 得到hocr的结果    上传  在这里全部上传
 */
void FullPicProof::on_getHocrButtom_clicked()
{
    //上传的地方，现在修改，必须是所有的mylistwidget都是灰色，才可以开始
    for(int i =0; i < myListWidget->count(); i++) {
        myListWidget->setCurrentRow(i);
        if(myListWidget->currentItem()->textColor() == Qt::black) {
            qDebug()<< "you have a black item";
            return;
        }
    }
    totalNumber = myListWidget->count();
    startDialogCount = 0;
    QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("是否上传？"));
    messagebox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    messagebox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
    messagebox.setButtonText(QMessageBox::Cancel,QStringLiteral("取 消"));
    int reply = messagebox.exec();
    qDebug()<<reply;

    if(reply == 1024){
        //everythings to Go 把本地数据库里所有的东西上传
        this->receiveEverythingFromPicsToGo();
    } else {

    }
}

/**
 * @brief FullPicProof::okToGo  去保存
 */
void FullPicProof::okToGo()
{
    totalNumber = myListWidget->count();
    picUpload->NetRequestType = PicFinished;
    //保存的时候都要把 所有的 标点符号去除， flag为 true的 item  加进去 ， 每次加载的时候
    //这里有问题， 因为这里把行内的句逗号都 都给去除了，而只加上了行外的

    managePuncNodeToSave();

    //下面是对Node的转换
    itemrecognizer->constructHocrfromTextItemsNode(PageNode);
    for(PicEntityAndInfo* p: pics) {
        if(p->fileName == currentPic->fileName){
            p->fileOcr = itemrecognizer->getHocr();
        }
    }

    QString exec_str = QString("UPDATE tb_book SET hocr = '%1'   WHERE filename = '%2'").arg(itemrecognizer->getHocr()).arg(currentPic->fileName);
    database->query->exec(exec_str);

    //picUpload->fullpicupload(this->userName,"7",currentPic->fullName,currentPic->fileCode,itemrecognizer->getHocr());   //这里是错的，还没改
    uploadFinishedRespose();
}

//测试
void FullPicProof::on_testButton_clicked()
{
    textScene->testSceneRectToSeeIfItsRight();
}

/**
 * @brief FullPicProof::uploadFinishedRespose  上传后做出的反应，除了各种变色，存储，还有申请下一张图片
 */
void FullPicProof::uploadFinishedRespose()
{
    currentPic->isFinishedOrNot = true;
    QString  exec_str = QString("UPDATE tb_book SET isFininshedOrNot = '1'   WHERE filename = '%2'").arg(currentPic->fileName);
    database->query->exec(exec_str);
    currentSeletedItem->setTextColor(Qt::gray);
    int index = myListWidget->currentRow();
    if(index + 1 == myListWidget->count()){
        QMessageBox overBox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("本次任务已完成，请上传"),QMessageBox::Ok);
        overBox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
        overBox.exec();
        return;
    }

    QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("保存成功"));
    messagebox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    messagebox.setButtonText(QMessageBox::Ok,QStringLiteral("下一张"));
    messagebox.setButtonText(QMessageBox::Cancel,QStringLiteral("确 定"));
    int reply = messagebox.exec();
    //qDebug()<<reply;
    if(reply == 1024){
        myListWidget->setCurrentRow(index + 1);
        downLoadPic(myListWidget->currentItem());
    }else {

    }
}

/**
 * @brief FullPicProof::initRatio  调控ratio
 */
void FullPicProof::initRatio()
{
    ratioSwitch = true;
    resizeRatioSwitch = false;
    ratio = 0;
    ratioIsChangedByResize = false;
    reSizeRatio = 0;
}

/**
 * @brief FullPicProof::resizeEvent  调控ratio
 * @param e
 */
void FullPicProof::resizeEvent(QResizeEvent *e)
{
    //%%
    if(ratioIsChangedByResize ){
        mView->scale(reSizeRatio,reSizeRatio);

        ratioIsChangedByResize = true;
        resizeRatioSwitch = false;
        reSizeRatio = mScene->height()/mView->height();
        mView->scale(1/reSizeRatio,1/reSizeRatio);

    }
    if(resizeRatioSwitch) {
        if(!ratioIsChangedByResize) {
            mView->scale(ratio,ratio);

        }
        reSizeRatio = mScene->height()/mView->height();
        mView->scale(1/reSizeRatio,1/reSizeRatio);
        resizeRatioSwitch = false;
        ratioSwitch = true;
        ratioIsChangedByResize = true;
        textView->setSceneRect(0,0,textView->width(),textView->height());

        //好这个地方比较草率啊，记得下次修改
        ColumnPic->setX(0);
        ColumnPic->setY(0);
        textView->update();
    }
}

/**
 * @brief FullPicProof::setRequestCountNumber  随时更改申请数
 * @param str
 */
void FullPicProof::setRequestCountNumber(QString str)
{
    requestNumber = str.toInt();
}

/**
 * @brief FullPicProof::SaveAndPass    保存到本地数据库里面
 */
void FullPicProof::SaveAndPass()
{
    //这里是用了没有加载的参数，来回绝不正常的 key_s  没有加载居然还保存
    if(ratio == 0) {
        QMessageBox overBox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("请您加载了信息在保存!!!!!!!"),QMessageBox::Ok);
        overBox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
        overBox.exec();
        return;
    }
    //这个方式是防止上传的时候 scene里那个  old new item切换时候崩溃的
    for(auto i:textScene->items()) {
        i->clearFocus();
    }
    //防止崩溃
    if(myListWidget->count() == 0) {
        QMessageBox messagebox;
        messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("无文件请勿上传"), QMessageBox::Yes);
        return;
    }

    if(this->textScene->items().size() == 0){
        QMessageBox messagebox;
        messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("无文件请勿上传"), QMessageBox::Yes);
        return;
    }
    this->okToGo();
}


void FullPicProof::keyPressEvent(QKeyEvent *event)
{

    //现在发现了，当焦点在 scene上的时候这里是不行的，必须点出去才可以，点一下listWidget才可以，没别的说的，在scene里弄一个信号和槽吧
    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_S) {
        SaveAndPass();
    }
}

/**
 * @brief FullPicProof::receiveEverythingFromPicsToGo  整体上传
 */
void FullPicProof::receiveEverythingFromPicsToGo()
{
    picUpload->NetRequestType = PicFinished;
    for (PicEntityAndInfo* i : pics) {
        picUpload->fullpicupload(this->userName,"7",i->fullName,i->fileCode,i->fileOcr);
    }
    //myListWidget->clear();   //不能在这里清除，需要在都返回那里都删除
}

/**
 * @brief FullPicProof::changeProgressBarSlots  进度条
 */
void FullPicProof::changeProgressBarSlots()
{
    //如果就一条，那就直接完毕
    if(totalNumber == 1){
        myListWidget->clear();
        database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '1'");
        return;
    }

    if(dialogOnOrNot == true) {
        //new这个dialog
        dialog = new QProgressDialog(QStringLiteral("正在上传，请稍后"),QStringLiteral("确定"),0,totalNumber - 1,this);
        dialog->setWindowTitle(QStringLiteral("进度对话框"));
        dialog->setAutoReset(false);
        dialog->setAutoClose(false);
    }
    dialogOnOrNot = false;

    if(startDialogCount == 0 ){

        dialog->show();
    }
    dialog->setValue(startDialogCount);
    startDialogCount ++;
    qDebug()<< "startDialogCount  ::::::::" << startDialogCount ;
    if(startDialogCount == totalNumber) {
        myListWidget->clear();
        database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '1'");
    }
}

/**
 * @brief FullPicProof::setWordNodeBeforePunc   这个是把所有的新建之前的node的beforePunc
 *    都设置为false，假如在解析那里重新载入的话，可能需要修改一下
 *
 *    那个是第一版，第二版是把所有的不是符号的都设置为 false
 *
 *    上面都作废了，重新的版本是新建node，把符号之前的都设为hasPuncNode 和  hasWhatPunc都写好了然后把
 */

void FullPicProof::setWordNodeBeforePunc()
{
    bool hasPunc = false;
    for(HocrNode * line : MainBodyNode->GetChildren()) {
        if(QString::fromStdString(line->GetNodeClass()) == "ocr_line"){
            if(line->GetChildren().size() == 0)
                continue;

            for(HocrNode* word: line->GetChildren()) {
                word->hasPuncNode = false;
                string q = word->GetCharacter();
                QString w = QString::fromLocal8Bit(q.c_str(), strlen(q.c_str()));
                if(w == "、" || w == "。" || w == "，") {
                    hasPunc = true;
                }
            }

            if(!true) {
                return;
            }
        }
    }
    //上面的代码应该整改一下，应该改为除了符号前面的都改为false  符号前面的改为true
    //并把符号node删除，hasWhatPunc赋值  word->hasPuncNode = false;
    managePuncNodeToReshowRight();
}

/**
 * @brief FullPicProof::managePuncNodeToSave 处理 生成的外部PuncNode  ...总而言之，这个是处理完就save
 */
void FullPicProof::managePuncNodeToSave()
{
    //把是的留下来，把不是的删除了， 把 node 里面是 标点符号
    //但是flag是false的删除了 ，其实可以删除的东西很多比方说 " " 和 "" 假如加入子行内的

    bool hasPunc = false;
    for(HocrNode* line : MainBodyNode->GetChildren()) {
        for(HocrNode* word : line->GetChildren()) {
            if(word->hasPuncNode == true) {
                hasPunc = true;
            }
        }
    }
    //先刹住，一会在开开
    if(!hasPunc){
        return;
    }
    //本来是计划除node，现在需要给pageNode 和  mainBodyNode 加属性
    //加上属性就卡死不了啦

    //下面这个是去除的,去除的是word里面   //这段注释已经没有用了
    HocrNode* newPageNode = new HocrNode;

    newPageNode->SetPhysicPageNo("ppageno 0");
    newPageNode->SetNodeClass("ocr_page");
    newPageNode->SetNodeId("page_1");
    newPageNode->SetBbox(PageNode->GetBbox().left,PageNode->GetBbox().top,
                         PageNode->GetBbox().right,PageNode->GetBbox().bottom);

    PageNode = newPageNode;

    HocrNode* newMainBody = new HocrNode;
    newMainBody->SetNodeClass("ocr_carea");
    newMainBody->SetNodeId("block_1_1");
    newMainBody->SetBbox(MainBodyNode->GetBbox().left,PageNode->GetBbox().top,
                         MainBodyNode->GetBbox().right,MainBodyNode->GetBbox().bottom);

    PageNode->AddChild(newMainBody);
    // QString q;
    // q = QString::fromLocal8Bit("、");
    // string s = q.toStdString();
    // qDebug() << QString::fromStdString(s);

    for(HocrNode* line: MainBodyNode->GetChildren()) {

        HocrNode* _line = new HocrNode;
        _line->SetNodeClass("ocr_line");
        _line->SetBbox(line->GetBbox().left,line->GetBbox().top,
                       line->GetBbox().right,line->GetBbox().bottom);

        if(QString::fromStdString(line->GetNodeClass()) == "ocr_line"){
            for(HocrNode* word: line->GetChildren()) {
                // ，。、  把所有的item里面  hasPunctuation  然后加进去  把hasPunctuation

                if(word->hasPuncNode) {
                    _line->AddChild(word);
                    HocrNode * puncNode = new HocrNode;

                    puncNode->SetNodeClass("ocrx_word");
                    //暂时把信心度设置为0，
                    puncNode->SetXWconf(100);
                    // puncNode->SetCharacter(word->hasWhatPunc);
                    // QString q;
                    // q = QString::fromLocal8Bit("、");
                    // string s = q.toStdString();
                    // qDebug() << QString::fromStdString(s);
                    string q = word->hasWhatPunc;
                    //QString w = QString::fromLocal8Bit(q.c_str(), strlen(q.c_str()));

                    puncNode->SetCharacter(q);
                    //puncNode->SetCharacter("、");
                    bbox wordBox = word->GetBbox();
                    // 设置punc Node的左上右下
                    puncNode->SetBbox(wordBox.left + (wordBox.right - wordBox.left)/2,
                                      wordBox.bottom - 20,
                                      wordBox.left + (wordBox.right - wordBox.left)/2 + 20,
                                      wordBox.bottom);
                    _line->AddChild(puncNode);
                }
                else {
                    _line->AddChild(word);
                }
            }
        }
        newMainBody->AddChild(_line);
    }
    MainBodyNode = newMainBody;

    //    //下面这个是增加的
    //    for(HocrNode* line: MainBodyNode->GetChildren()) {
    //        //if(line)
    //    }


    //把是的留下来，把不是的删除了， 把 node 里面是 标点符号
    //但是flag是false的删除了 ，其实可以删除的东西很多比方说 " " 和 "" 假如加入子行内的

}

/**
 * @brief FullPicProof::managePuncNodeToReshowRight
 *
 * 这里是为了重现，把所有的punc都归到前一个node里面
 */
void FullPicProof::managePuncNodeToReshowRight()
{
    HocrNode* newPageNode = new HocrNode;

    newPageNode->SetPhysicPageNo("ppageno 0");
    newPageNode->SetNodeClass("ocr_page");
    newPageNode->SetNodeId("page_1");
    newPageNode->SetBbox(PageNode->GetBbox().left,PageNode->GetBbox().top,
                         PageNode->GetBbox().right,PageNode->GetBbox().bottom);

    PageNode = newPageNode;

    HocrNode* newMainBody = new HocrNode;
    newMainBody->SetNodeClass("ocr_carea");
    newMainBody->SetNodeId("block_1_1");
    newMainBody->SetBbox(MainBodyNode->GetBbox().left,PageNode->GetBbox().top,
                         MainBodyNode->GetBbox().right,MainBodyNode->GetBbox().bottom);

    PageNode->AddChild(newMainBody);
    for(HocrNode* line: MainBodyNode->GetChildren()) {

        HocrNode* _line = new HocrNode;
        _line->SetNodeClass("ocr_line");
        _line->SetBbox(line->GetBbox().left,line->GetBbox().top,
                       line->GetBbox().right,line->GetBbox().bottom);

        if(QString::fromStdString(line->GetNodeClass()) == "ocr_line"){
            //之后需要测试一下，是不是值溢出了      //这种情况下有一个弊端，
            for(int i = 0 ; i < line->GetChildren().size();i ++) {
                // ，。、  把所有的item里面  hasPunctuation  然后加进去  把hasPunctuation
                if(i + 1 < line->GetChildren().size()) {
                    string q = (line->GetChildren().at(i+1))->GetCharacter();
                    QString w = QString::fromStdString(q);
                    qDebug() << "####:" <<w;
                    qDebug() << "@@@@:" <<"，";
                    // if (QString::compare(w, d) == 0) {
                    if(w == QStringLiteral("、") || w == QStringLiteral("，")|| w == QStringLiteral("。")){
                        (line->GetChildren().at(i))->hasPuncNode = true;
                        (line->GetChildren().at(i))->hasWhatPunc = q;  //w.toStdString();
                        _line->AddChild(line->GetChildren().at(i));
                        i++;
                        continue;
                    }
                }
                _line->AddChild(line->GetChildren().at(i));
            }
        }
        newMainBody->AddChild(_line);
    }
    MainBodyNode = newMainBody;
}








