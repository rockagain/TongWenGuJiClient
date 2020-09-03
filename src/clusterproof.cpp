#include "clusterproof.h"
#include "ui_clusterproof.h"


ClusterProof::ClusterProof(QString projectid, QWidget *parent, QString username) :
    QWidget(parent),
    ui(new Ui::ClusterProof)
{
    ui->setupUi(this);
    this->userName = username;
    //设置各种框
    leftView = new MyView;
    leftView->setBoardType(CLUSTERPROOF);
    uperView = new MyView;
    uperView->setBoardType(CLUSTERPROOF);
    uperView->setStyleSheet("background-color:#eeeeee;");
    underView = new MyView;
    underView->setBoardType(CLUSTERPROOF);
    underView->setStyleSheet("background-color:#eeeeee;");
    leftScene = new MyTextScene;
    leftScene->setBoardType(CLUSTERPROOF);
    uperScene  = new MyScene;
    uperScene->setBoardtype(CLUSTERPROOF);
    underScene = new MyScene;
    leftView->setScene(leftScene);
    uperView->setScene(uperScene);
    underView->setScene(underScene);
    //添加splitter里面的内容
    leftSplitter = new QSplitter(Qt::Horizontal);
    leftSplitter->addWidget(leftView);
    rightSplitter = new QSplitter(Qt::Vertical,leftSplitter);
    rightSplitter->addWidget(uperView);
    //设置字体的大小为50
    lineEdit = new QLineEdit;
    //设置里面的字体
    QFont font;
    font.setPixelSize(75);
    lineEdit->setFont(font);
    //添加splitter里面的内容
    cornerSplitter = new QSplitter(Qt::Horizontal);
    rightSplitter->addWidget(cornerSplitter);
    cornerSplitter->addWidget(underView);
    cornerSplitter->addWidget(lineEdit);
    //设置lineEdit的大小，好固定下面的形状
    lineEdit->setFixedSize(80,80);

    //设置splitter里面的关系
    rightSplitter->setStretchFactor(0,7);
    rightSplitter->setStretchFactor(1,1);
    cornerSplitter->setStretchFactor(0,7);
    cornerSplitter->setStretchFactor(1,1);
    leftSplitter->setStretchFactor(0,1);
    leftSplitter->setStretchFactor(1,7);
    //设置下面view的大小，才可以显示正常
    leftView->resize(1920/7*3,900);
    //    //&#   没用
    //    uperScene->setSceneRect(0,0,uperView->width(),uperView->height());
    //把splitter插进去
    ui->verticalLayout->addWidget(leftSplitter);
    //申请网络申请对象
    netRequest = new NetWorkClub(this->userName);
    netDownLoad = new NetWorkClub(this->userName);  //这两个之后可以改过来
    picUpload = new NetWorkClub(this->userName);
    //设置滚动条
    leftView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //&
    //totalNumber = 3;  //下载的数量    //要和数据库里面的一致  sqlite
    //建立数据库函数
    picDbRequest();
    //给这个最左边的框子排上字
    leftScene->setView(leftView);
    //给项目ID赋值
    projectId = projectid;
    //获取底下的scrollbar
    QScrollBar * bar = leftView->horizontalScrollBar();
    bar->setVisible(false);
    //把开关开开
    dialogOnOrNot = true;
    //
    startDialogCount = 0;
    //只有一张图片，且这张图上没有字，我们这里假定它为true.这里的picture的意思就是说没有字，并不一定只是picture
    onlyPicture = true;
    //信号和槽
    connect(netRequest,&NetWorkClub::setClusterRequestInfo,this,&ClusterProof::receiveClustRequestToGo);
    connect(netDownLoad,&NetWorkClub::sentPicInfo,this,&ClusterProof::receivePicInfoToStore);
    connect(lineEdit,&QLineEdit::textChanged,this,&ClusterProof::textChangedResponse);
    connect(leftScene,&MyTextScene::focusItemChanged,uperScene,&MyScene::clusterLeftSceneShow);
    connect(netRequest,&NetWorkClub::sentClusterProgressToGo,this,&ClusterProof::changeProgressBarSlots);
    connect(picUpload,&NetWorkClub::sentClusterFinishedProgressToGo,this,&ClusterProof::changeFinishedProgressBarSlots);
    connect(uperScene,&MyScene::setClusterProofsLineEditFocus,this,&ClusterProof::setLineEditFocused);
}

ClusterProof::~ClusterProof()
{
    delete ui;
}

void ClusterProof::on_requestButton_clicked()
{
    if(leftScene->items().size() != 0) {
        QMessageBox messagebox;
        messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("请上传后再申请"), QMessageBox::Yes);
        return;
    }
    dialogOnOrNot = true;
    totalNumber = requestNumber;
    startDialogCount = 0;   //这里赋值应该就解决了之前的问题
    QString num = QString::number(totalNumber,10);   // 把 total 转化为10进制
    netRequest->NetRequestType = ClusterRequest;
    netRequest->clustproofRequest(projectId,this->userName,num);
}

void ClusterProof::receiveClustRequestToGo(QString info)
{
    this->downloadOksignal();
    QJsonObject obj = netRequest->getJsonObjectFromString(info);
    QJsonValueRef  ref = obj["fileList"];
    QJsonArray arrary =  ref.toArray();

    for(int i = 0; i < arrary.size() ; i++) {
        //分析 返回的 all
        QJsonValueRef rf = arrary[i];
        QJsonObject ob =  rf.toObject();
        QString fullname = ob["fileName"].toString();
        QFileInfo mFile(fullname);
        QString filename;
        filename = mFile.fileName().replace(mFile.suffix(),"");
        filename = filename.left(filename.length() - 1);
        //填充book信息
        pic = new PicEntityAndInfo;
        // pic->id = ob["id"].toString();
        pic->fileCode =  QString::number(ob["fileId"].toInt(),10);
        pic->fullName = fullname;
        pic->fileName = filename;
        pic->filePath = ob["path"].toString();
        pic->isFinishedOrNot = false;
        pic->fileOcr = ob["hocrText"].toString();
        if(pic->fileOcr == ""){
            QMessageBox messagebox;
            messagebox.information(NULL,QStringLiteral("提示"),QStringLiteral("hocr内容为空,数据管理出现错误"), QMessageBox::Yes);
            QMessageBox errorfile;
            errorfile.information(NULL,QStringLiteral("errorfile"),"hocr为空的文件为  ：" +pic->fileName, QMessageBox::Yes);
            //改为continue 就可以其余的都上传了
            startDialogCount ++ ;
            continue;
        }
        pic->picPageNode =  ItemRecognizer::changeHocrToPageNode(pic->fileOcr);
        pics.append(pic);
        downloadPicName = pic->filePath; // + "//" + pic->fullName;     //服务器端改了
        downloadPicName.replace("\\","//");

        netDownLoad->NetRequestType = PicDownload;
        netDownLoad->downLoad(this->userName,downloadPicName);
    }
    //下载完毕后显示提示框，前面这是之前的注释，现在又要换了   这里把所有的pic存起来  //等等
}

void ClusterProof::receiveDatabaseToGo(QVector<PicEntityAndInfo*> pics)
{
    this->downloadOksignal();
    totalNumber = pics.size();   //整体的数据

    for(int i = 0; i < pics.size() ; i++) {

        downloadPicName = pics.at(i)->filePath; // + "//" + pic->fullName;     //服务器端改了
        //一个变量所引发的血案
        pics.at(i)->picPageNode =  ItemRecognizer::changeHocrToPageNode(pics.at(i)->fileOcr);
        netDownLoad->NetRequestType = PicDownload;
        netDownLoad->downLoad(this->userName,pics.at(i)->filePath.replace("\\","//"));
    }
    //下载完毕后显示提示框，前面这是之前的注释，现在又要换了   这里把所有的pic存起来  //等等
}

/**
 * @brief ClusterProof::receivePicInfoToStore  //下载好了图片了
 * @param info
 */
void ClusterProof::receivePicInfoToStore(QString info)
{

    QJsonObject obj = netRequest->getJsonObjectFromString(info);
    QJsonValueRef  ref = obj["fileInfo"];

    QString picStr = ref.toString();   //这里记录一下，以后好告诉对方明确得数据类型
    QByteArray bytearray;
    bytearray += picStr;
    QByteArray picArray;
    picArray = picArray.fromBase64(bytearray);
    QJsonValueRef filename = obj["fileName"];
    QString name = filename.toString();
    //设置uperscene的形状，把scene扩大
    //uperScene->setSceneRect(0,0,1000,800);  //这行还下面的一些行都是要移动到别处的
    uperScene->setSceneRect(0,0,uperView->width(),uperView->height());
    //必须申请，不申请不好使了，可能是因为跨模块的原因
    bool isInsertOrNot = database->isDataBaseEmptyOrNot();
    for (PicEntityAndInfo *p : pics) {
        /**************处理文件名****************/
        QString filename;
        filename = removeFileNamesSuffix(name);
        /*******************************/
        //qDebug() << "$$$$$$$$$$$$$$$$$$$$$$" << filename << " " << p->fileName;
        if (p->fileName == filename ) {
            p->base64 = picStr;
        }
        if(isInsertOrNot) {
            //@@在这里把这个图片的信息存在数据库里面
            QString exc_str = QString("INSERT INTO tb_book VALUES('%1','%2','%3','%4','%5','%6')").arg(p->fileCode).arg(p->fullName).arg(p->fileName).arg(p->filePath).arg(p->isFinishedOrNot).arg(p->fileOcr);
            qDebug()<<"database:::";
            qDebug()<<database->query->exec(exc_str);
        }
    }
    //emit 槽函数
    changeProgressBarSlots();
}

QString ClusterProof::removeFileNamesSuffix(QString name)
{
    QFileInfo mFile(name);
    QString filename;
    filename = mFile.fileName().replace(mFile.suffix(),"");
    filename = filename.left(filename.length() - 1);
    return filename;
}

//接收了图片们的信息收展示出来   //规定好了接口 就需要添加参数
void ClusterProof::receivePicsToFillTheScene()
{

}

void ClusterProof::textChangedResponse(QString text)
{
    if (text.size() == 1) {
        currentRightWord = text;
        //qDebug()<<text;
    }
}

void ClusterProof::resizeEvent(QResizeEvent *event)
{
    // uperScene->setSceneRect(0,0,uperView->width() - 10,uperView->height() - 10);
    // uperScene->setSceneRect(0,0,uperView->width() - 10,uperView->height() + 1000);
    leftScene->setSceneRect(0,0,leftView->width() - 10,leftView->height());
    //判断字们的长度是否比leftview要大
    if(leftScene->items().size()*80 > leftView->height()){
        leftScene->setSceneRect(0,0,leftView->width() - 10,leftScene->items().size()*80 + 80);
    }
    handlePicture();
}

/**
 * @brief ClusterProof::handleThePictureToMakeItShowRight  处理图片叫他们显示在正确的位置
 * @param pixmapItems
 */
void ClusterProof::handlePicture()
{
    int row = 0;
    int column = 0;
    for(int i = 0;i < uperScene->items().size(); i++,column++) {
        if ((column - 1) * 110 < uperView->width()) {
            uperScene->items().at(i)->setPos(column * 110, row * 110);
        }
        if ((column + 2) * 110 > uperView->width()) {
            row += 1;
            column = -1;
        }
    }
    if((100 * row ) > uperView->height()) {
        uperScene->setSceneRect(0,0,uperView->width(),110*row +100);
    } else {
        uperScene->setSceneRect(0,0,uperView->width(),uperView->height());
    }


    leftScene->setSceneRect(0,0,leftView->width() - 10,leftView->height());
    //判断字们的长度是否比leftview要大
    if(leftScene->items().size()*80 > leftView->height()){
        leftScene->setSceneRect(0,0,leftView->width() - 10,leftScene->items().size()*80 + 80);
    }
}

/**
 * @brief ClusterProof::changeProgressBarSlots  下载时候的进度条
 */
void ClusterProof::changeProgressBarSlots()
{


    if(dialogOnOrNot == true ) {
        //new这个dialog 只搞一次
        dialog = new QProgressDialog(QStringLiteral("正在下载图片，请稍后"),QStringLiteral("确定"),0,totalNumber - 1,this);
        dialog->setWindowTitle(QStringLiteral("进度对话框"));
        dialog->setAutoReset(false);
        dialog->setAutoClose(false);
    }
    dialogOnOrNot = false;

    if(pic->fileOcr != ""){
        qDebug()<< "hocr为空";
    }
    if(startDialogCount == 0 ){

        dialog->show();
    }
    dialog->setValue(startDialogCount);
    startDialogCount ++;
    qDebug()<< "startDialogCount  ::::::::" << startDialogCount ;
    if(totalNumber == startDialogCount) {
        showThePicAndText();
        //dialog->hide();
        startDialogCount = 0;
    }
}

/**
 * @brief ClusterProof::changeFinishedProgressBarSlots 完事时候的进度条
 */
void ClusterProof::changeFinishedProgressBarSlots()
{
    if(totalNumber == 1){
        return;
    }

    if(uploadFinishedDialog == 0){

        finishedDialog->show();
    }

    finishedDialog->setValue(uploadFinishedDialog);
    uploadFinishedDialog ++;
    //    if(totalNumber == uploadFinishedDialog - 1 ) {

    //        uploadFinishedDialog = 0;
    //    }
}

//当计数结束时，把所有下载的字都显示出来 名字有歧义    //
void ClusterProof::showThePicAndText()
{
    CharAndCharRegionListVector = new QVector<CharAndCharRegionList *>;
    // 切割图片 QVector<PicEntityAndInfo*> pics;
    for (int i = 0; i < pics.size(); i++) {
        QByteArray bytearray;
        //这个地方出过一个bug，一个变量名字用的最后的临时变量，所以所有图片都是最后一张
        bytearray += pics[i]->base64;
        QByteArray picArray;
        picArray = picArray.fromBase64(bytearray);
        QImage image;
        image.loadFromData(picArray);
        //HocrNode *_node = pics.at(i)->picPageNode;
        //qDebug() << QString::fromStdString(_node->GetNodeClass()) << endl;
        //没有文字，只能直接处理,直接pass就可以了啊、
        HocrNode* mainBodyNode = pics.at(i)->picPageNode->GetChildren().at(0);
        for(HocrNode* lineNode : mainBodyNode->GetChildren()) {
            if(QString::fromStdString(lineNode->GetNodeClass()) == "ocr_line"){
                onlyPicture = false;
            }
        }
        if(onlyPicture && (pics.size() == 1)){

            picUpload->NetRequestType = PicFinished;
            //把函数转化过来
            ItemRecognizer* itemrecognizer = new ItemRecognizer(uperScene);
            itemrecognizer->pageNode = pics.at(0)->picPageNode;
            itemrecognizer->constructHocrfromTextItemsNode();    //这个函数直接用的，到时候查看一下
            QString hocr = itemrecognizer->getHocr().replace("\n","\\n").toHtmlEscaped();

            picUpload->clusterProofupload(this->userName,"6",pics.at(0)->fullName,pics.at(0)->fileCode,hocr);

            leftScene->clear();
            uperScene->clear();
            pics.clear();
            leftScene->words.clear();
            //把这个数给设置一下
            uploadFinishedDialog = 0;
            //@@这里数据库清空，把所有的东西都删除了
            database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '0'");
            QMessageBox messagebox;
            messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("里面只有一张图片，且已经被上传了"), QMessageBox::Yes);
            return;
        }
        if(onlyPicture){
            //假如真的只有图片的格式，那就不用处理了
            continue;
        }
        //有文字的node，就直接识别没有的在上面处理
        ItemRecognizer::fromNodeFillInMaps(image, pics.at(i)->picPageNode, CharAndCharRegionListVector, i);
    }

    // 显示
    for(CharAndCharRegionList* list : *CharAndCharRegionListVector ) {
        static int i = 0;   //个人观察了一下，这里这个静态变量设置的没什么道理，不静态的就可以啊  //后来证明了，不静态的就不可以

        if (leftScene->items().size() == 0){
            i = 0;
        }
        //        if(list->character == "" || list->character.size() != 1) {
        //            continue;
        //            //这个地方不能光continue就完事了，后面应该出处理方案
        //        }

        //--------------------------------------------------------------------------------------------------------------------------------
        //这里下面这两个 if  是 保证 聚类校对正常显示的纠正 ，这个地方也别封装了，等有时间的代码不变动的时候再封装吧
        if(list->character == "")
            list->character = "*";
        //现在只有在你的聚类校对上不校对的时候才可以OK
        if(list->character.size() != 1)
            list->character = list->character.left(1);
        //------------------------------------------------------------------------------------------------------------------------------------


        ClusterTextItem* textItem = new ClusterTextItem;
        // connect(textItem,&MyTextItem::)
        textItem->setFlag(QGraphicsTextItem::ItemIsFocusable);
        textItem->setPlainText(list->character);
        textItem->setDefaultTextColor(Qt::black);
        textItem->setItemSize(80);
        textItem->setPos(0, i);
        textItem->setCharAndPicList(list);
        leftScene->addItem(textItem);
        //把每一个字都放在向下的event words 里面
        leftScene->words.append(textItem);
        i += 80;
    }
    //算字的长度
    if(leftScene->items().size()*80 > leftView->height()){
        leftScene->setSceneRect(0,0,leftView->width() - 10,leftScene->items().size()*80 + 80);
    }
}

/**
 * @brief ClusterProof::keyPressEvent   向下敲回车，然后调整字啊
 * @param event
 */
void ClusterProof::keyPressEvent(QKeyEvent *event)
{
    if (((event->key() == Qt::Key_Return )||(event->key() == Qt::Key_Enter))
            &&(uperScene->items().size() != 0)) {
        //qDebug()<<currentRightWord;
        if(currentRightWord != ""){
            fixNodeAndVector();
        }
        lineEdit->clear();
        //就在这里剔除那个字,说的就是左边里面的那个多余的，没有图片了的字
        if(uperScene->items().size() == 0) {
            //第一个for循环是记录单字的颜色的
            for(CharAndCharRegionList* listAndChaColor: *CharAndCharRegionListVector){
                for(auto i: leftScene->items()){
                    if(listAndChaColor->character == ((MyTextItem*)(i))->toPlainText())
                        listAndChaColor->textColor = ((MyTextItem*)(i))->defaultTextColor();
                }
            }

            for(CharAndCharRegionList* list : *CharAndCharRegionListVector ) {
                if(list->character == leftScene->currentTextItem->charAndPicLists->character){
                    CharAndCharRegionListVector->removeOne(list);
                    //这个应该删除
                    leftScene->words.clear();
                    uperScene->clear();
                    int leftCharacterOrder;
                    for(int i = 0; i < leftScene->items().size(); i++){
                        if(leftScene->items().at(i)->hasFocus()) {
                            leftCharacterOrder = i;
                            leftScene->items().at(i)->clearFocus();
                        }
                    }
                    leftScene->clear();
                    int value = leftView->bar->value();
                    showLeftCharacter();
                    leftView->bar->setValue(value);
                    break;
                }
            }
        }
    }
    //现在发现了，当焦点在 scene上的时候这里是不行的，必须点出去才可以，点一下listWidget才可以，没别的说的，在scene里弄一个信号和槽吧
    //上面这段注释是别的模块的，把上传那里的hocr 换成保存就可以了
    if((event->modifiers() == Qt::ControlModifier) && event->key() == Qt::Key_S) {
        /*************************************************************************************************************/
        if(leftScene->items().size() == 0) {
            QMessageBox messagebox;
            messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("无文件请勿保存"), QMessageBox::Yes);
            return;
        }
        //        for(auto i : leftScene->items()){
        //            if(i->hasFocus()){
        //                i->clearFocus();
        //            }
        //        }
        QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("是否保存"));
        messagebox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        messagebox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
        messagebox.setButtonText(QMessageBox::Cancel,QStringLiteral("取 消"));
        int reply = messagebox.exec();
        qDebug()<<reply;

        if(reply == 1024){

        } else {
            return;
        }

        for(PicEntityAndInfo* pic:pics) {
            //把函数转化过来
            ItemRecognizer* itemrecognizer = new ItemRecognizer(uperScene);
            itemrecognizer->pageNode = pic->picPageNode;
            itemrecognizer->constructHocrfromTextItemsNode();    //这个函数直接用的，到时候查看一下
            QString hocr = (itemrecognizer->getHocr())/*.replace("\n","\\n").toHtmlEscaped()*/;

            //picUpload->clusterProofupload(this->userName,"6",pic->fullName,pic->fileCode,hocr);
            //把上面这个上传去了，修正下面这个这个sql 就是保存每一个hocr
            //database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '0'");
            QString exec_str = QString("UPDATE tb_book SET hocr = '%1'   WHERE filename = '%2'").arg(hocr).arg(pic->fileName);
            database->query->exec(exec_str);
        }
        /*******************************************************************************************/
    }
}

void ClusterProof::setRequestNumber(int value)
{
    requestNumber = value;
}
/**
 * @brief ClusterProof::fixNodeAndVector   这里得vector 说的是图片单字和它得图片得vector，改了错字
 */
void ClusterProof::fixNodeAndVector()
{
    int value = leftView->bar->value();
    for (int i = 0 ; i < uperScene->items().size(); i++ ) {
        if(uperScene->items().at(i)->isSelected()){
            MyPixmapItem* pixItem = (MyPixmapItem*)(uperScene->items().at(i));
            //????这个index是干嘛的啊？？？？
            int pixIndex = pixItem->picInfo->index;
            this->fixNode(pixIndex,pixItem->picInfo->bbox);
            this->fixVector(currentRightWord,pixItem->picInfo);
            uperScene->removeItem(uperScene->items().at(i));
            //假如要添加聚焦下一张图片就在这里聚焦,新加得代码
        }
    }
    //就是这里出了问题
    //这里解决了这个问题
    for(auto i : leftScene->items()){
        i->clearFocus();
    }
    //不知道这里为什么这么关键去了就不好使了..//切换图片的地方   还得优化啊 这个地方

    leftScene->currentTextItem->setFocus();
    if(uperScene->items().size() != 0) {
        uperScene->items().at(0)->setFocus();
        uperScene->items().at(0)->setSelected(true);
    }
    //        if(leftScene->keyPressdInt != 0){
    //            leftScene->items().at(leftScene->keyPressdInt)->setFocus();
    //            leftScene->items().at(leftScene->keyPressdInt)->setSelected(true);
    //        }

    //设置滚动条的value
    //leftView->bar->setValue(leftView->scrollvalue*80);
    leftView->bar->setValue(value);
}

/**
 * @brief ClusterProof::fixNode  这个函数是找到目标的node改数据
 * @param index
 */
void ClusterProof::fixNode(int index,hocr::bbox bbox)
{
    HocrNode* mainNode = pics.at(index)->picPageNode->GetChildren().at(0);
    for(HocrNode* lineNode : mainNode->GetChildren()) {
        for(HocrNode* wordNode : lineNode->GetChildren()) {
            if(isBboxEqual(wordNode->GetBbox(),bbox)) {
                wordNode->SetCharacter(currentRightWord.toStdString());
            }
        }
    }
}
/**
 * @brief ClusterProof::fixVector  把vector里面的数据都更正了
 * @param word
 * @param picInfo
 */
void ClusterProof::fixVector(QString word,CharPicInfo *picInfo)
{
    //删除错误的vector里面的内容
    for(int i = 0 ; i < leftScene->items().size() ; i++) {
        //下面这句的意思就是，错了的时候，看哪里包含他，之后下面再移除它，下面就是这个判断
        if(((MyTextItem*)(leftScene->items().at(i)))->charAndPicLists->regionList.contains(picInfo)) {
            ((MyTextItem*)(leftScene->items().at(i)))->charAndPicLists->regionList.removeOne(picInfo);
        }
    }

    //把信息添加到正确的位置上去
    for(int i = 0 ; i < leftScene->items().size() ; i++) {
        if(((MyTextItem*)(leftScene->items().at(i)))->charAndPicLists->character == word) {
            ((MyTextItem*)(leftScene->items().at(i)))->charAndPicLists->regionList.append(picInfo);
            return;
        }
    }

    ClusterTextItem* textItem = new ClusterTextItem;
    // connect(textItem,&MyTextItem::)
    textItem->setFlag(QGraphicsTextItem::ItemIsFocusable);
    textItem->setPlainText(word);
    //    textItem->setDefaultTextColor(Qt::red);
    textItem->setItemSize(80);
    CharAndCharRegionList* list = new CharAndCharRegionList;
    textItem->setCharAndPicList(list);
    textItem->charAndPicLists->regionList.append(picInfo);
    leftScene->addItem(textItem);
    textItem->setPos(0,leftScene->items().size()*80 - 80);
    textItem->charAndPicLists->character = word;
    //个人觉得显示不正确可能就是这里没弄对////之前显示少一个字也是这个原因就是在底子里没加上这个字，，实在是不应该啊//终于把bug解决了
    CharAndCharRegionListVector->append(textItem->charAndPicLists);
    handlePicture();
}

/**
 * @brief ClusterProof::isBboxEqual   看两个Bbox是否相等
 * @param box1
 * @param box2
 * @return
 */
bool ClusterProof::isBboxEqual(hocr::bbox box1,hocr::bbox box2)
{
    if(box1.left == box2.left && box1.right == box2.right && box1.bottom == box2.bottom && box1.top == box2.top){
        return true;
    } else
        return false;
    //写成下面这样好一点
    //    int flag = 0;
    //    box1.left == box2.left ? flag += 1 : flag -= 1;
    //    box1.top == box2.top ? flag += 1 : flag -= 1;
    //    box1.right == box2.right ? flag += 1 : flag -= 1;
    //    box1.bottom == box2.bottom ? flag += 1 : flag -= 1;

    //    if (flag == 4)
    //        return true;

    //    return false;
}

//这里没写完
void ClusterProof::on_upDateButtom_clicked()
{
    if(leftScene->items().size() == 0) {
        QMessageBox messagebox;
        messagebox.information(NULL, QStringLiteral("提示"),QStringLiteral("无文件请勿上传"), QMessageBox::Yes);
        return;
    }
    for(auto i : leftScene->items()){
        if(i->hasFocus()){
            i->clearFocus();
        }
    }
    QMessageBox messagebox(QMessageBox::NoIcon,QStringLiteral("提示"),QStringLiteral("是否上传？"));
    messagebox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    messagebox.setButtonText(QMessageBox::Ok,QStringLiteral("确 定"));
    messagebox.setButtonText(QMessageBox::Cancel,QStringLiteral("取 消"));
    int reply = messagebox.exec();
    qDebug()<<reply;

    if(reply == 1024){

    } else {
        return;
    }
    //当totalNumber等于1的时候特殊处理
    if(totalNumber == 1) {
        for(PicEntityAndInfo* pic:pics) {
            picUpload->NetRequestType = PicFinished;
            //把函数转化过来
            ItemRecognizer* itemrecognizer = new ItemRecognizer(uperScene);
            itemrecognizer->pageNode = pic->picPageNode;
            itemrecognizer->constructHocrfromTextItemsNode();    //这个函数直接用的，到时候查看一下
            QString hocr = itemrecognizer->getHocr().replace("\n","\\n").toHtmlEscaped();

            picUpload->clusterProofupload(this->userName,"6",pic->fullName,pic->fileCode,hocr);
        }
        leftScene->clear();
        uperScene->clear();
        pics.clear();
        leftScene->words.clear();
        //把这个数给设置一下
        uploadFinishedDialog = 0;
        //@@这里数据库清空，把所有的东西都删除了
        database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '0'");
        //把两个变量设置好
        leftScene->keyDownInt = 0;
        leftScene->keyUpInt = 0;

        return;
    }

    finishedDialog = new QProgressDialog(QStringLiteral("正在上传图片，请稍后"),QStringLiteral("确定"),0,totalNumber - 1 ,this);
    finishedDialog->setAutoClose(false);
    finishedDialog->setAutoReset(false);
    finishedDialog->setWindowTitle(QStringLiteral("进度对话框"));

    for(PicEntityAndInfo* pic:pics) {
        picUpload->NetRequestType = PicFinished;
        //把函数转化过来
        ItemRecognizer* itemrecognizer = new ItemRecognizer(uperScene);
        itemrecognizer->pageNode = pic->picPageNode;
        itemrecognizer->constructHocrfromTextItemsNode();    //这个函数直接用的，到时候查看一下
        QString hocr = itemrecognizer->getHocr().replace("\n","\\n").toHtmlEscaped();

        picUpload->clusterProofupload(this->userName,"6",pic->fullName,pic->fileCode,hocr);
    }
    leftScene->clear();
    uperScene->clear();
    pics.clear();
    leftScene->words.clear();
    //把这个数给设置一下
    uploadFinishedDialog = 0;
    //@@这里数据库清空，把所有的东西都删除了
    database->query->exec("DELETE FROM tb_book WHERE isFininshedOrNot = '0'");
}

void ClusterProof::picDbRequest()
{
    database = new MyDatabase("./clusterProof.db");
    if(!database->isDataBaseEmptyOrNot()){
        //这里是个bug
        qDebug() <<QDir::currentPath();
        qDebug() << "the database is what"<< !database->isDataBaseEmptyOrNot();
        qDebug() << database->query->value(0);
        //这个方法是根据查询sqlite得到结果后，生成书得信息得，这里得封装得方法和  申请那里不同，那里没有搞
        constructPic();
        while(database->query->next()) {
            qDebug() << database->query->value(0);
            constructPic();
        }
        qDebug()<<"yesssssssssssssssssssssssssssssssssssssss";
        receiveDatabaseToGo(pics);
    }

}

void ClusterProof::setLineEditFocused()
{
    lineEdit->setFocusPolicy(Qt::StrongFocus);
    lineEdit->setFocus(Qt::OtherFocusReason);
}

void ClusterProof::constructPic()
{
    pic = new PicEntityAndInfo;
    pic->fileCode = database->query->value(0).toString();
    pic->fullName = database->query->value(1).toString();
    pic->fileName = database->query->value(2).toString();
    pic->filePath = database->query->value(3).toString();
    pic->isFinishedOrNot = database->query->value(4).toBool();
    pic->fileOcr = database->query->value(5).toString();
    pics.append(pic);
}

/**
 * @brief ClusterProof::showLeftCharacter   添加左侧的字
 */
void ClusterProof::showLeftCharacter()
{
    for(CharAndCharRegionList* list : *CharAndCharRegionListVector ) {
        //static int i = 0;   //个人观察了一下，这里这个静态变量设置的没什么道理，不静态的就可以啊  ???????为啥不可以
        static int i = 0;
        if (leftScene->items().size() == 0){
            i = 0;
        }
        //--------------------------------------------------------------------------------------------------------------------------
        //        if(list->character == "" || list->character.size() != 1) {
        //            continue;
        //            //这个地方不能光continue就完事了，后面应该出处理方案
        //        }
        //这里下面这两个 if  是 保证 聚类校对正常显示的纠正 ，这个地方也别封装了，等有时间的代码不变动的时候再封装吧
        //--------------------------------------------------------------------------------------------------------------------------
        if(list->character == "")
            list->character = "*";
        //现在只有在你的聚类校对上不校对的时候才可以OK
        if(list->character.size() != 1)
            list->character = list->character.left(1);

        ClusterTextItem* textItem = new ClusterTextItem;
        // connect(textItem,&MyTextItem::)
        textItem->setFlag(QGraphicsTextItem::ItemIsFocusable);
        textItem->setPlainText(list->character);
        textItem->setDefaultTextColor(Qt::black);
        textItem->setItemSize(80);
        textItem->setPos(0, i);
        textItem->setCharAndPicList(list);
        textItem->setDefaultTextColor(list->textColor);
        leftScene->addItem(textItem);
        //把每一个字都放在向下的event  里面
        leftScene->words.append(textItem);
        i += 80;
    }
    //算字的长度
    if(leftScene->items().size()*80 > leftView->height()){
        leftScene->setSceneRect(0,0,leftView->width() - 10,leftScene->items().size()*80 + 80);
    }

}

void ClusterProof::setRequestCountNumber(QString str)
{
    requestNumber = str.toInt();
}



