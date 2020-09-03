#include "editboard.h"
#include "ui_editboard.h"

/**
 * @brief 主画版
 * @param parent
 */
EditBoard::EditBoard(QString projectid, QWidget *parent, QString username):
    QWidget(parent),
    ui(new Ui::EditBoard)
{
    ui->setupUi(this);
    //view set scene
    this->userName = username;
    //projectId 赋值
    projectId = projectid;

    mView = new MyView;
    mScene = new MyScene;
    mScene->setBoardtype(EDITBOARD);
    mView->setBoardType(EDITBOARD);

    mScene->setView(mView);    //这句是没有用的，到时候要被删除
    mView->setScene(mScene);
    mView->setStyleSheet("background-color:#eeeeee;");

    //设置填充editboard的splitter
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
    //做editboard的网络申请
    picDbRequest();
    //把这个值设置为0 的意思是 一开始不运行resizeEvent
    fitWidth = 0;
    //后续添加的item识别模块，复用之前写的itemrecognizer
    recognizer = new ItemRecognizer(mScene);
    //下载的图片 ratio 开关,用来调控ratio的东西用到了很多变量，也不知道可不可以处理不这么烂
    initRatio();
    //设置一上来就可以画
    this->setMainBodyFlag();
    //dialog的开关
    dialogOnOrNot = true;
    //进度条的起始点
    startDialogCount = 0;
    //横竖bar    这里有点扭曲是   mScene里的
    mView->hbar = mView->horizontalScrollBar();
    mView->vbar = mView->verticalScrollBar();

    //信号和槽
    connect(netRequest, &NetWorkClub::sentItemsName,this, &EditBoard::receiveItemName);
    connect(myListWidget, &MyListWidget::itemDoubleClicked,this,&EditBoard::downLoadPic);
    connect(netDownLoad, &NetWorkClub::sentPicInfo,this,&EditBoard::receivePicInfo);
    //connect(mScene,&MyScene::sceneSendHocr,this,&EditBoard::receiveHocrToGo);
    connect(picUpload,&NetWorkClub::uploadFinishedSignal,this,&EditBoard::changeProgressBarSlots);
    //connect(mScene,&MyScene::setBoardOnFire,this,&EditBoard::receiveHocrToGo);
}

EditBoard::~EditBoard()
{
    delete ui;
}

void EditBoard::setToolbar(QToolBar *value)
{
    toolbar = value;
}

/**
 * @brief 申请  图片信息
 */
void EditBoard::on_requestButton_clicked()
{

    if(database->isDataBaseEmptyOrNot()) {
        totalNumber = requestNumber;
        QString num = QString::number(totalNumber,10);
        //调节开关重置一下
        dialogOnOrNot = true;
        //pics清空，否则就重复了
        pics.clear();
        //申请图片数量自己调
        netRequest->NetRequestType = PicRequest;
        //&
        netRequest->editboardRequest(projectId,this->userName,num);  //数量
    }
    //    else if(testToSeeIfEeveryPicIsFinished()) {
    //        //调节开关重置一下
    //        dialogOnOrNot = true;
    //        //这里的意思就是把弄完的都删除，然后申请新的
    //        pics.clear();
    //        myListWidget->clear();
    //        database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '1'");
    //        netRequest->NetRequestType = PicRequest;
    //        netRequest->editboardRequest(projectId,this->userName,num);
    //        //这里的意思是新换一批的时候，把所有的item删除
    //        for (QGraphicsItem* item : mScene->items()) {
    //            mScene->removeItem(item);
    //        }
    //    }
}

/**
 * @brief EditBoard::on_zoomOutButton_clicked
 */
void EditBoard::on_zoomOutButton_clicked()
{
    mScene->setMode(ManageMode::ZOOMOUT);
    mScene->views().first()->scale(0.9,0.9);
    mScene->setMode(ManageMode::EDIT);
}

/**
 * @brief 通过按钮设置模式
 */
void EditBoard::on_zoomInButton_clicked()
{
    mScene->setMode(ManageMode::ZOOMIN);
    mScene->views().first()->scale(1.1,1.1);
    mScene->setMode(ManageMode::EDIT);
}

/**
 * @brief 通过按钮设置模式
 */
void EditBoard::on_editButton_clicked()
{
    mScene->setMode(ManageMode::EDIT);
    mView->setFocus();
    mScene->setFocus();//scene 设置focus
}

/**
 * @brief 这个是Item的颜色，来区分遮盖的
 */
void EditBoard::on_setColorButton_clicked()
{
    mScene->setColorToCheck();
}

/**
 * @brief EditBoard::combobox的index改变，然后触发这个函数，设置Item外沿color，还有其实里面把mode也改了
 * @param index
 */
void EditBoard::on_comboBox_currentIndexChanged(int index)
{
    mScene->setPenColorAndMode(index);
    mView->setFocus();
}

/**
 * @brief 得到hocr的结果
 */
void EditBoard::on_getHocrButtom_clicked()
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
 * @brief 这个函数是申请图片信息的
 */
void EditBoard::picDbRequest()
{
    database = new MyDatabase("./editBoard.db");
    //下面写读取的东西  的过程， 意思就是 query 一下  空就申请，不空就读取里面的东西，然后显示

    if(!database->isDataBaseEmptyOrNot()){

        qDebug() << database->query->value(0);
        //这个方法是根据查询sqlite得到结果后，生成书得信息得，这里得封装得方法和  申请那里不同，那里没有搞
        constructPic();
        while(database->query->next()) {
            qDebug() << database->query->value(0);
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

/**
 * @brief 信号槽返回的，建立listwidget的图片的所有的信息的
 * @param all
 */
void EditBoard::receiveItemName(QString all)
{

    QJsonObject obj = netRequest->getJsonObjectFromString(all);
    QJsonValueRef  ref = obj["fileList"];
    QJsonArray arrary =  ref.toArray();
    for(int i = 0; i < arrary.size() ; i++) {

        //分析 返回的 all
        QJsonValueRef rf = arrary[i];
        QJsonObject ob =  rf.toObject();
        QString fullname = ob["name"].toString();
        QListWidgetItem* item = new QListWidgetItem;


        QString filename =  ClusterProof::removeFileNamesSuffix(fullname); //第一次用到代码里 log一下
        //这里是说的是一开始是黑的地方
        item->setTextColor(Qt::black);
        item->setText(filename);
        if (i % 2 == 0)
            item->setBackground(QBrush(QColor("#eaeaea")));
        myListWidget->addItem(item);
        //填充book信息
        pic = new PicEntityAndInfo;

        // pic->id = ob["id"].toString();
        //pic->fileCode = ob["id"].toString();
        pic->fileCode = QString::number(ob["id"].toInt(),10);
        pic->fullName = fullname;
        pic->fileName = filename;
        pic->filePath = ob["path"].toString();
        pic->isFinishedOrNot = false;
        pics.append(pic);

        //把数据都存到 数据库内
        QString exc_str = QString("INSERT INTO tb_book VALUES('%1','%2','%3','%4','%5','%6')").arg(pic->fileCode).arg(pic->fullName).arg(pic->fileName).arg(pic->filePath).arg(pic->isFinishedOrNot).arg("");
        qDebug()<<database->query->exec(exc_str);
    }
    //下载完毕后的提示框
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
/**
 * @brief 槽函数，点击边框侧图片名字被触发
 * @param item
 */
void EditBoard::downLoadPic(QListWidgetItem *item)
{
    //找啊找
    for(PicEntityAndInfo* i :pics) {
        if(i->fileName == item->text()) {
            downloadPicName = i->filePath;  //+ "//" + i->fullName;  //服务器改了
            downloadPicName.replace("\\","//");
            //qDebug()<<downloadPicName<<"&&&&&"<<"@@@";
            currentPic = i;
            break;
        }
    }
    //qDebug()<<downloadPicName;
    netDownLoad->downLoad(this->userName,downloadPicName);

    this->downloadOksignal();
}

/**
 * @brief EditBoard::downLoadPic 下载图片的函数，这个函数只用于申请的时候下载第一张图片
 */
void EditBoard::downLoadtheFirstPic()
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
void EditBoard::receivePicInfo(QString all)
{
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
    mScene->setSceneRect(0, 0, image->width(), image->height());
    mScene->drawBackground(mPainter, mRectf);
    //下面这一段(if里面的)是处理一开始下载下来的图片的显示的，要 刚刚好
    //%%
    ratio = mScene->height()/mView->height();
    if(ratioSwitch){
        if(ratiolIsChangedByResize){
            mView->scale(reSizeRatio,reSizeRatio);
            ratiolIsChangedByResize = false;
        }
        mView->scale(1/ratio ,1/ratio );
        ratioSwitch = false;
    }
    //写在这里为真是因为一上来就resize的话，那就啥也没有了
    resizeRatioSwitch = true;

    mView->update();

    //把刚才的item保存下来，去之后那里把它拉灰，就可以了 ,接收图片后就赋值，这样就好一些
    //从这里是最科学的
    currentSeletedItem = new QListWidgetItem;
    currentSeletedItem =  myListWidget->currentItem();

    //转换还原hocr格式
    if (currentPic->fileOcr == ""){
        return;
    }
    QString hocr = ProofBoard::convertTheHocrFormats(currentPic->fileOcr);
    //##  在这里形成item就可以了
    //int temper = mScene->nodeMode;     //这里这个没用
    //识别
    recognizer->makePicItemfromHocr(hocr);
    //这里也该搞一搞  搞了以后就统一了
    this->setMainBodyFlag();

}

//查询数据可，生成书的信息
void EditBoard::constructPic()
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

//处理 item 对象的
void EditBoard::constructPicItem(QString filename,bool isFinishedOrNot)
{
    QListWidgetItem* item = new QListWidgetItem;
    //这个地方是开始的时候测试是否是黑还是灰的地方
    if(isFinishedOrNot) {
        item->setTextColor(Qt::gray);
    } else {
        item->setTextColor(Qt::black);
    }

    item->setText(filename);
    myListWidget->addItem(item);
}

//接收了hocr，然后保存，上传
void EditBoard::receiveHocrToGo()
{
    //这里是用了没有加载的参数，来回绝不正常的 key_s  没有加载居然还保存
    if(ratio == 0) {
        QMessageBox overBox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("请您加载了信息在保存!!!!!!!"),QMessageBox::Ok);
        overBox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
        overBox.exec();
        return;
    }
    //在这里统一的更改坐标,  我觉得这个函数可以封装一下啊
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
    //移植之前写一小迁移前的注意， 一  检测item，假如是空的画还是默认没反应。  二是tohtml那个函数，也就是转义，不转义的话搞不定啊
    if(mScene->items().size() == 0)
        return;
    recognizer->setImage(this->image);
    //生成
    recognizer->constructNodefromPicItem();
    //下面这个生成hocr的函数重载了换成新的了,新的连图片和table表格也能认识，parser封装的比较好
    //QString hocr =  recognizer->constructHocrfromNode();

    QString  hocr = "";
    hocr = recognizer->constructHocrfromNode(recognizer->rootNode);

    qDebug() << "======" <<hocr;  //这里遇见了个小坑，意思就是我后来写的函数里面已经转了html格式的，但是我复用的时候忘记了，导致了teacher李那里服务崩溃了
    if(hocr == "you have a error") {
        QMessageBox messagebox;
        messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("请再检查画框结构,或联系您的维护人员"), QMessageBox::Yes);
        return;
    }

    //数据可保存，并上传
    QString exec_str = QString("UPDATE tb_book SET hocr = '%1'   WHERE filename = '%2'").arg(hocr).arg(currentPic->fileName);
    database->query->exec(exec_str);
    //qDebug()<<hocr;

    for(PicEntityAndInfo* p: pics) {
        if(p->fileName == currentPic->fileName){
            p->fileOcr = hocr;
        }
    }
    //  最后集体上传,在上传那里搞个循环，然后，你懂的
    //    picUpload->NetRequestType = PicFinished;
    //    picUpload->editboardupload(this->userName,"",currentPic->fullName,currentPic->fileCode,hocr);
    netFinishedReponse();
}

//测试所有的图片是否都已经处理完毕
bool EditBoard::testToSeeIfEeveryPicIsFinished()
{
    for(PicEntityAndInfo* p: pics) {
        if (!p->isFinishedOrNot) {
            return false;
        }
    }
    return true;

}

/**
 * @brief EditBoard::netFinishedReponse 上传后做出的反应，除了各种变色，存储，还有申请下一张图片
 */
void EditBoard::netFinishedReponse()
{
    //这个item是刚才接收到的信息，然后变gray
    currentSeletedItem->setTextColor(Qt::gray);
    currentPic->isFinishedOrNot = true;
    //把更改后的结果存入数据库
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
    qDebug()<<reply;

    if(reply == 1024){
        myListWidget->setCurrentRow(index + 1);
        downLoadPic(myListWidget->currentItem());
        //在这里重置一下flag，否则下一张不显示蓝色
        mScene->colorOrNot = true;
    } else {

    }
}


void EditBoard::resizeEvent(QResizeEvent *e)
{
    //%%
    if(ratiolIsChangedByResize ){
        mView->scale(reSizeRatio,reSizeRatio);
        ratiolIsChangedByResize = true;
        resizeRatioSwitch = false;
        reSizeRatio = mScene->height()/mView->height();
        mView->scale(1/reSizeRatio,1/reSizeRatio);
    }
    if(resizeRatioSwitch) {

        if(!ratiolIsChangedByResize) {
            mView->scale(ratio,ratio);
        }
        reSizeRatio = mScene->height()/mView->height();
        mView->scale(1/reSizeRatio,1/reSizeRatio);
        resizeRatioSwitch = false;
        ratioSwitch = true;
        ratiolIsChangedByResize = true;
    }

}


void EditBoard::setBigCharFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(BIG_CHAR);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            //下面这些应当封装一个函数的，有时间就封装一下
            item->setMode(SINGLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,true,true,true,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void EditBoard::setSingleRowFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(SINGLE_ROW);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,true,true,true,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,EDITBOARD);
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
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void EditBoard::setMainBodyFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(MAIN_BODY);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,true,true,true,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
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
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void EditBoard::setTableFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(TABLE);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == TABLE){
            item->setMode(TABLE,false,true,true,true,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == BIG_CHAR){
            item->setMode(BIG_CHAR,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == MAIN_BODY){
            item->setMode(MAIN_BODY,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == DOUBLE_ROW){
            item->setMode(DOUBLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}

void EditBoard::setImageFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(IMAGE);
    for(int i = 0 ; i < mScene->items().size() ; i++) {
        MyItem *item = (MyItem*)(mScene->items().at(i));
        if(item->nodeMode == SINGLE_ROW){
            item->setMode(SINGLE_ROW,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == IMAGE){
            item->setMode(IMAGE,false,true,true,true,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
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
            item->setMode(SMALL_CHAR,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}


void EditBoard::setDoubleRowFlag()
{
    mScene->isCreateOrIsHover = false;
    mScene->mode = ManageMode::EDIT;
    mScene->setPenColorAndMode(DOUBLE_ROW);
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
            item->setMode(DOUBLE_ROW,false,true,true,true,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
        if(item->nodeMode == SMALL_CHAR){
            item->setMode(SMALL_CHAR,false,false,false,false,EDITBOARD);
            item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            item->setFlag(QGraphicsItem::ItemIsMovable,false);
        }
    }
}


void EditBoard::setSmallCharFlag()
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

void EditBoard::setRequestNumber(int value)
{
    requestNumber = value;
}

/**
 * @brief EditBoard::initRatio   初始化ratio相关的参数
 */
void EditBoard::initRatio()
{
    ratioSwitch = true;
    resizeRatioSwitch = false;
    ratio = 0;
    ratiolIsChangedByResize = false;
    reSizeRatio = 0;
}


void EditBoard::keyPressEvent(QKeyEvent *event)
{
    //现在发现了，当焦点在 scene上的时候这里是不行的，必须点出去才可以，点一下listWidget才可以，没别的说的，在scene里弄一个信号和槽吧
    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_S) {
        receiveHocrToGo();
    }
}

/**
 * @brief EditBoard::receiveEverythingFromSqliteToGo  这个函数的意思是把所有sqlite里面的数据上传至服务器然后清空
 */
void EditBoard::receiveEverythingFromPicsToGo()
{
    picUpload->NetRequestType = PicFinished;
    for (PicEntityAndInfo* i : pics) {
        picUpload->editboardupload(this->userName,"",i->fullName,i->fileCode,i->fileOcr);
    }
    //myListWidget->clear();   //不能在这里清除，需要在都返回那里都删除
}

/**
 * @brief EditBoard::changeProgressBarSlots  进度条
 */
void EditBoard::changeProgressBarSlots()
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


void EditBoard::setRequestCountNumber(QString str)
{
    requestNumber = str.toInt();
}

