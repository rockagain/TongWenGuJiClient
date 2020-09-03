#include "proofboard.h"
#include "ui_proofboard.h"
#include <iostream>
using namespace tinyxml2;
using namespace std;

ProofBoard::ProofBoard(QString projectid, QWidget *parent, QString userName) :
    QWidget(parent),
    ui(new Ui::ProofBoard)
{
    ui->setupUi(this);
    this->userName = userName;
    totalNumber = 3;

    mView = new MyView();
    mScene = new MyScene;
    mView->setScene(mScene);
    mScene->setBoardtype(PROOFBOARD);
    mView->setBoardType(PROOFBOARD);
    mView->setStyleSheet("background-color:#eeeeee;");

    dialogOnOrNot = true;

    //设置填充proofboard的splitter
    myListWidget = new MyListWidget;
    splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(myListWidget);
    splitter->addWidget(mView);
    ui->verticalLayout->addWidget(splitter,Qt::AlignCenter);
    splitter->setStretchFactor(1,8);

    netRequest = new NetWorkClub(this->userName);
    netDownLoad = new NetWorkClub(this->userName);
    picUpload = new NetWorkClub(this->userName);
    netDownLoad->NetRequestType = PicDownload;
    //识别者
    itemrecognizer = new ItemRecognizer(mScene);
    //初始化一些管ratio的参数
    initRatio();
    //projectId 赋值
    projectId = projectid;
    //数据库相关
    picDbRequest();
    //集体设置一下flag
    this->setBigCharFlag();
    //去控制框子的调控
    mView->hbar = mView->horizontalScrollBar();
    mView->vbar = mView->verticalScrollBar();
    //信号和槽
    connect(netRequest, &NetWorkClub::setProofRequestInfo,this, &ProofBoard::receiveProofRequestInfo);
    connect(myListWidget, &MyListWidget::itemDoubleClicked,this,&ProofBoard::downLoadPic);
    connect(netDownLoad, &NetWorkClub::sentPicInfo,this,&ProofBoard::receivePicInfo);
    //这个需要改把 signal 改为 识别者那里的就可以了     按钮那里也需要改  不调用那边的函数了
    connect(itemrecognizer,&ItemRecognizer::recognizerSendHocr,this,&ProofBoard::receiveHocrToGo);
    connect(picUpload,&NetWorkClub::uploadFinishedSignal,this,&ProofBoard::changeProgressBarSlots);
    //connect(mScene,&MyScene::setBoardOnFire,this,&ProofBoard::generateAndSaveHocrToGo);
}

ProofBoard::~ProofBoard()
{
    delete ui;
}

void ProofBoard::picDbRequest()
{
    database = new MyDatabase("./proofBoard.db");
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
void ProofBoard::constructPic()
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

void ProofBoard::constructPicItem(QString filename,bool isFinishedOrNot)
{
    QListWidgetItem* item = new QListWidgetItem;
    //开始的时候的颜色
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
void ProofBoard::on_requestButton_clicked()
{

    if(database->isDataBaseEmptyOrNot()) {
        totalNumber = requestNumber;
        QString num = QString::number(totalNumber,10);
        dialogOnOrNot = true;
        pics.clear();
        //申请图片数量自己调
        netRequest->NetRequestType = ProofRequest;
        //&
        netRequest->proofBoardRequest(projectId,this->userName,num);   //申请图片
    }
    //    else if(testToSeeIfEeveryPicIsFinished()) {          //这里的意思就是把弄完的都删除，然后申请新的
    //        pics.clear();
    //        myListWidget->clear();
    //        database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '1'");
    //        netRequest->NetRequestType = ProofRequest;
    //        netRequest->proofBoardRequest(projectId,this->userName,num);
    //        //这里的意思是新换一批的时候，把所有的item删除
    //        for (QGraphicsItem* item : mScene->items()) {
    //            mScene->removeItem(item);
    //        }
    //    }
}

/***************************和绘画有关的，还有和图片放大缩小等按钮等***********************************************************/

//放大
void ProofBoard::on_zoomOutButton_clicked()
{
    mScene->setMode(ManageMode::ZOOMOUT);
    mScene->views().first()->scale(0.9,0.9);
    mScene->setMode(ManageMode::EDIT);
}

/**
 * @brief 通过按钮设置模式
 */
void ProofBoard::on_zoomInButton_clicked()
{
    mScene->setMode(ManageMode::ZOOMIN);
    mScene->views().first()->scale(1.1,1.1);
    mScene->setMode(ManageMode::EDIT);
}

/**
 * @brief 通过按钮设置模式
 */
void ProofBoard::on_editButton_clicked()
{
    mScene->setMode(ManageMode::EDIT);
    mView->setFocus();
}

/**
 * @brief 这个是Item的颜色，来区分遮盖的
 */
void ProofBoard::on_setColorButton_clicked()
{
    mScene->setColorToCheck();
}

/**
 * @brief combobox的index改变，然后触发这个函数，设置Item外沿color，还有其实里面把mode也改了
 * @param index
 */
void ProofBoard::on_comboBox_currentIndexChanged(int index)
{
    mScene->setPenColorAndMode(index);
    mView->setFocus();
}

/**
 * @brief 得到hocr的结果   这里是集体上传的地方
 */
void ProofBoard::on_getHocrButtom_clicked()
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

/*************************************************************************************************************************/
// 轮查图片的状态
bool ProofBoard::testToSeeIfEeveryPicIsFinished()
{
    for(PicEntityAndInfo* p: pics) {
        if (!p->isFinishedOrNot) {
            return false;
        }
    }
    return true;
}

//收取proofquest 的信息
void ProofBoard::receiveProofRequestInfo(QString info)
{
    qDebug()<<info;
    QJsonObject obj = netRequest->getJsonObjectFromString(info);
    QJsonValueRef  ref = obj["bmjdApplyList"];
    QJsonArray arrary =  ref.toArray();
    for(int i = 0; i < arrary.size() ; i++) {
        //分析 返回的 info
        QJsonValueRef rf = arrary[i];
        QJsonObject ob =  rf.toObject();
        QString fullname = ob["fileName"].toString();
        QListWidgetItem* item = new QListWidgetItem;

        QString filename;
        filename = ClusterProof::removeFileNamesSuffix(fullname);
        item->setTextColor(Qt::black);
        item->setText(filename);
        if (i % 2 == 0)
            item->setBackground(QBrush(QColor("#eaeaea")));
        myListWidget->addItem(item);
        //填充book信息
        pic = new PicEntityAndInfo;

        //pic->fileCode = ob["fileId"].toString();   //fileID
        pic->fileCode  = QString::number(ob["fileId"].toInt(),10);
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
    //申请完后的提示框
    QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("申请成功"));
    messagebox.setStandardButtons(QMessageBox::Ok/*|QMessageBox::Cancel*/);
    messagebox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
    //messagebox.setButtonText(QMessageBox::Cancel,QStringLiteral("下一张"));
    int reply = messagebox.exec();
    //qDebug()<<reply;
    // mScene->clear();
    for(auto i : mScene->items()) {
        mScene->removeItem(i);
    }
    if(reply == 1024){
        downLoadtheFirstPic();
    }
}

//download 下载图片
void ProofBoard::downLoadPic(QListWidgetItem *item)
{  
    for(auto i : mScene->items()) {
        mScene->removeItem(i);
    }
    netDownLoad->NetRequestType = PicDownload;
    for(PicEntityAndInfo* i :pics) {
        if(i->fileName == item->text()) {
            downloadPicName = i->filePath ;  //+ "//" + i->fullName;  //服务器变了
            downloadPicName.replace("\\","//");

            currentPic = i;
            break;
        }
    }
    netDownLoad->downLoad(this->userName,downloadPicName);
    this->downloadOksignal();
}

/**
 * @brief ProofBoard::downLoadtheFirstPic  下载第一张图片，只有在申请的时候检测
 */
void ProofBoard::downLoadtheFirstPic()
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
 * @brief 信号槽返回的，获得图片内容信息的
 * @param all
 */
void ProofBoard::receivePicInfo(QString all)
{
    //qDebug()<<all;

    //这里的意思是接收新图片的时候，把所有的item删除
    if(mScene->items().size() != 0 ) {
        for (QGraphicsItem* item : mScene->items()) {
            mScene->removeItem(item);
        }
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
    mScene->setSceneRect(0, 0, image->width(), image->height());
    mScene->drawBackground(mPainter,mRectf);
    //初始化ratio
    ratio = mScene->height()/mView->height();
    if(ratioSwitch){
        if(ratioIsChangedByResize){
            mView->scale(reSizeRatio,reSizeRatio);
            ratioIsChangedByResize = false;
        }
        mView->scale(1/ratio ,1/ratio );
        ratioSwitch = false;
    }
    resizeRatioSwitch = true;

    mView->update();

    //把刚才的item保存下来，去之后那里把它拉灰，就可以了 ,接收图片后就赋值，这样就好一些
    //从这里是最科学的
    currentSeletedItem = new QListWidgetItem;
    currentSeletedItem =  myListWidget->currentItem();

    mScene->update(0,0,image->width(),image->height());
    changeHocrToItem();

}

//接收了hocr，然后保存，上传
void ProofBoard::receiveHocrToGo(QString hocr)
{
    QString exec_str = QString("UPDATE tb_book SET hocr = '%1'   WHERE filename = '%2'").arg(hocr).arg(currentPic->fileName);
    database->query->exec(exec_str);
    //qDebug()<<hocr;
    picUpload->NetRequestType = PicFinished;
    //下面这段代码是为了更新pics里面的hocr信息，否则你画完一张，然后再跳回去，那张还是那个样！
    for(PicEntityAndInfo* p: pics) {
        if(p->fileName == currentPic->fileName){
            p->fileOcr = hocr;
        }
    }
    uploadFininsdResponse();
    //picUpload->proofboardupload(this->userName,"",currentPic->fullName,currentPic->fileCode,hocr);
}

//把传回来的hocr，转换为Item对象    版面校对的地方用
void ProofBoard::changeHocrToItem()
{

    if(currentPic->fileOcr == "")
        return;
    QString hocr =  convertTheHocrFormats(currentPic->fileOcr);
    //在产生item之前 先清空一下
    //temper 存的是nodemode  也就是当前的变量
    itemrecognizer->makePicItemfromHocr(hocr);
    this->setBigCharFlag();

}

QString ProofBoard::convertTheHocrFormats(QString hocr)
{
    hocr.replace("&lt;","<");
    hocr.replace("&gt;",">");
    hocr.replace("&quot;", "\"");
    //hocr 所有的 \n 转化为 \\n
    hocr.replace("\\n","\n");
    return hocr;
}

/**
 * @brief ProofBoard::uploadFininsdResponse   上传后做出的反应，除了各种变色，存储，还有申请下一张图片
 */
void ProofBoard::uploadFininsdResponse()
{
    currentSeletedItem->setTextColor(Qt::gray);
    currentPic->isFinishedOrNot = true;
    QString exec_str = QString("UPDATE tb_book SET isFininshedOrNot = '1'   WHERE filename = '%2'").arg(currentPic->fileName);
    database->query->exec(exec_str);
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
        //在这里重置一下flag，否则下一站不显示蓝色了
        mScene->colorOrNot = true;
    }else {

    }

}

void ProofBoard::setBigCharFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(BIG_CHAR);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,true,true,true,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void ProofBoard::setSingleRowFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(SINGLE_ROW);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,true,true,true,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void ProofBoard::setMainBodyFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(MAIN_BODY);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,true,true,true,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void ProofBoard::setTableFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(TABLE);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,true,true,true,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void ProofBoard::setImageFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(IMAGE);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,true,true,true,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void ProofBoard::setDoubleRowFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(DOUBLE_ROW);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,true,true,true,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,PROOFBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

/**
 * @brief proofBoard::setSmallCharFlag
 */
void ProofBoard::setSmallCharFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(SMALL_CHAR);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,true,true,true,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}


void ProofBoard::setRequestNumber(int value)
{
    requestNumber = value;
}

/**
 * @brief ProofBoard::initRatio  管理ratio的
 */
void ProofBoard::initRatio()
{
    ratioSwitch = true;
    resizeRatioSwitch = false;
    ratio = 0;
    ratioIsChangedByResize = false;
    reSizeRatio = 0;
}

/**
 * @brief ProofBoard::resizeEvent  resize 里面调试了ratio的大小
 * @param e
 */
void ProofBoard::resizeEvent(QResizeEvent *e)
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
    }
}


/**
 * @brief ProofBoard::setRequestCountNumber  槽函数， 跟着mainwindow 改申请数的
 * @param str
 */
void ProofBoard::setRequestCountNumber(QString str)
{
    requestNumber = str.toInt();
}

/**
 * @brief ProofBoard::keyPressEvent  在这里调用触发  ， 把以前上传那里 移到了这里
 * @param event
 */
void ProofBoard::keyPressEvent(QKeyEvent *event)
{

    //现在发现了，当焦点在 scene上的时候这里是不行的，必须点出去才可以，点一下listWidget才可以，没别的说的，在scene里弄一个信号和槽吧
    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_S) {
        generateAndSaveHocrToGo();
    }
}


void ProofBoard::receiveEverythingFromPicsToGo()
{
    picUpload->NetRequestType = PicFinished;
    for (PicEntityAndInfo* i : pics) {
        picUpload->proofboardupload(this->userName,"",i->fullName,i->fileCode,i->fileOcr);
    }
    //myListWidget->clear();   //不能在这里清除，需要在都返回那里都删除
}



void ProofBoard::changeProgressBarSlots()
{
    //如果就一条，那就直接完毕
    if(totalNumber == 1){
        myListWidget->clear();
        database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '1'");
        return;
    }
    if(dialogOnOrNot == true) {
        //new这个dialog 只搞一次
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

void ProofBoard::generateAndSaveHocrToGo()
{

    //这里是用了没有加载的参数，来回绝不正常的 key_s  没有加载居然还保存
    if(ratio == 0) {
        QMessageBox overBox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("请您加载了信息在保存!!!!!!!"),QMessageBox::Ok);
        overBox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
        overBox.exec();
        return;
    }

    if(myListWidget->count() == 0) {
        QMessageBox messagebox;
        messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("无文件请勿上传"), QMessageBox::Yes);
        return;
    }

    if(this->mScene->items().size() == 0){
        QMessageBox messagebox;
        messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("无文件请勿上次"), QMessageBox::Yes);
        return;
    }
    //    QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("是否上传？"));
    //    messagebox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    //    messagebox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
    //    messagebox.setButtonText(QMessageBox::Cancel,QStringLiteral("取 消"));
    //    int reply = messagebox.exec();
    //    qDebug()<<reply;

    //    if(reply == 1024){
    //上面的都注释了，这个不需要验证了，直接保存数据库

    //这段代码应该封装一下
    for(auto i : mScene->items()){
        ((MyItem*)i)->hide();
        if(((MyItem*)i)->getIsCheckOkOrNot()){
            ((MyItem*)i)->checkPoint();
            //((MyItem*)i)->hide();
            ((MyItem*)i)->setRect(((MyItem*)i)->start.x()
                                  ,((MyItem*)i)->start.y()
                                  ,((MyItem*)i)->rect().width()
                                  ,((MyItem*)i)->rect().height());
        }
    }

    itemrecognizer->setImage(this->image);
    itemrecognizer->constructNodefromPicItem();
    itemrecognizer->constructHocrfromNode(itemrecognizer->rootNode);
    itemrecognizer->myItems.clear();
    itemrecognizer->myCharItems.clear();

}



