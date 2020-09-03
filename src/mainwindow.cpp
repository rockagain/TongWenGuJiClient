#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkclub.h"


MainWindow::MainWindow(QString projectId, QWidget *parent, QString nodecode, QString userName) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置toolbar的格式
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //这个是登陆传过来的nodecode，用这个来判断显示哪个工序
    nodeCode = nodecode;
    this->userName = userName;

    broadcastNumberEdit = new QLineEdit;
    broadcastNumberEdit->setText("5");

    //给项目id赋值传给每个板子，接着传给每个函数
    projectid = projectId;
    //改名字
    this->setWindowTitle(QStringLiteral("通文古籍数字化加工平台客户端"));
    //判断加载哪个工序,选择哪个板子去显示
    chooseWhatBoard();
    //this->setFixedSize(1920,1000);
    //添加菜单栏等
    addMenuAndBar();
    //toolbar工具栏
    addToolbarAction();

    connect(quitAction,&QAction::triggered,this,&MainWindow::close);
    //menubar的信号和槽
    connect(clearSqlAction,&QAction::triggered,this,&MainWindow::clearSqlActionConnect);
    connect(aboutAction,&QAction::triggered,this,&MainWindow::showTheIntroduction);
    //toolbar的信号和槽
    connect(requestAction,&QAction::triggered,this,&MainWindow::requestActionResponse);
    connect(uploadAction,&QAction::triggered,this,&MainWindow::uploadActionResponse);
    connect(zoomOutAction,&QAction::triggered,this,&MainWindow::zoomOutActionResponse);
    connect(zoomInAction,&QAction::triggered,this,&MainWindow::zoomInActionResponse);
    connect(setCoverAction,&QAction::triggered,this,&MainWindow::setCoverActionResponse);
    connect(mainBodyAction,&QAction::triggered,this,&MainWindow::mainBodyActionResponse);
    connect(imageAction,&QAction::triggered,this,&MainWindow::imageActionResponse);
    connect(annotationAction,&QAction::triggered,this,&MainWindow::annotationActionResponse);
    connect(tableAction,&QAction::triggered,this,&MainWindow::tableActionResponse);
    connect(singleRowAction,&QAction::triggered,this,&MainWindow::singleRowActionResponse);
    connect(doubleRowAction,&QAction::triggered,this,&MainWindow::doubleRowActionResponse);
    connect(bigCharAction,&QAction::triggered,this,&MainWindow::bigCharActionResponse);
    connect(smallCharAction,&QAction::triggered,this,&MainWindow::smallCharActionResponse);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::setBoardType 设置板子的属性
 * @param value
 */
void MainWindow::setBoardType(int value)
{
    BoardType = value;
}
/***********************下面这些函数是初始化版子，设置板子的************************************************/
void MainWindow::startEditBoard()
{
    editboard = new EditBoard(projectid,0,this->userName);
    connect(editboard,&EditBoard::downloadOksignal,this,&MainWindow::showToolBarAction);
    setCentralWidget(editboard);
    this->setBoardType(EDITBOARD);
    //给editboard 那里的requestNumber 设定值
    editboard->setRequestNumber((broadcastNumberEdit->text().toInt()));
    //建立信号和槽
    connect(broadcastNumberEdit,&QLineEdit::textChanged,editboard,&EditBoard::setRequestCountNumber);
}

void MainWindow::startProofBoard()
{
    proofboard = new ProofBoard(projectid,0,this->userName);
    connect(proofboard,&ProofBoard::downloadOksignal,this,&MainWindow::showToolBarAction);
    setCentralWidget(proofboard);
    this->setBoardType(PROOFBOARD);
    proofboard->setRequestNumber((broadcastNumberEdit->text().toInt()));
    connect(broadcastNumberEdit,&QLineEdit::textChanged,proofboard,&ProofBoard::setRequestCountNumber);
}

void MainWindow::startFullPicBoard()
{
    wholepicboard = new FullPicProof(projectid,0,this->userName);
    connect(wholepicboard,&FullPicProof::downloadOksignal,this,&MainWindow::showToolBarAction);
    setCentralWidget(wholepicboard);
    this->setBoardType(FULLPICPROOF);
    wholepicboard->setRequestNumber((broadcastNumberEdit->text().toInt()));
    connect(broadcastNumberEdit,&QLineEdit::textChanged,wholepicboard,&FullPicProof::setRequestCountNumber);
}

void MainWindow::startClusterBoard()
{
    clustproof = new ClusterProof(projectid,0,this->userName);
    connect(clustproof,&ClusterProof::downloadOksignal,this,&MainWindow::showToolBarAction);
    setCentralWidget(clustproof);
    this->setBoardType(CLUSTERPROOF);
    clustproof->setRequestNumber((broadcastNumberEdit->text().toInt()));
    //建立信号和槽
    connect(broadcastNumberEdit,&QLineEdit::textChanged,clustproof,&ClusterProof::setRequestCountNumber);
}
/*************************************************************************/
/**
 * @brief MainWindow::clearSqlActionConnect 清空数据库链接
 */
void MainWindow::clearSqlActionConnect()
{
    switch (this->BoardType) {
    case EDITBOARD:
        editboard->database->query->exec("delete from tb_book");
        editboard->myListWidget->clear();
        break;
    case PROOFBOARD:
        proofboard->database->query->exec("delete from tb_book");
        proofboard->myListWidget->clear();
        break;
    case CLUSTERPROOF:
        clustproof->database->query->exec("delete from tb_book");
        //clustproof->myListWidget->clear();
        break;
    case FULLPICPROOF:
        wholepicboard->database->query->exec("delete from tb_book");
        wholepicboard->myListWidget->clear();
        break;
    default:
        break;
    }
}

/**
 * @brief MainWindow::showTheIntroduction 通文数字加工平台的简介
 */
void MainWindow::showTheIntroduction()
{
    QMessageBox overBox(QMessageBox::NoIcon,
                        QStringLiteral("About 通文古籍数字化加工平台"),
                        QStringLiteral("<b>ocr-client-1.3.18-release-20180516</b><br><br>我们的数字平台是一个基于机器学习的古文加工系统<br><br><b>目标: 效率,稳定!!</b>"),
                        QMessageBox::Ok);
    overBox.setButtonText(QMessageBox::Ok, QStringLiteral("OK"));
    overBox.exec();
    return;
}

/**
 * @brief MainWindow::chooseWhatBoard 选择哪个板子去显示
 */
void MainWindow::chooseWhatBoard()
{
    //画版
    if(this->nodeCode == "hb") {
        this->startEditBoard();
        this->setBoardType(EDITBOARD);
    } //版面校对
    else if (this->nodeCode == "bmjd") {
        this->startProofBoard();
        this->setBoardType(PROOFBOARD);
    } //整夜校对
    else if (this->nodeCode == "zyjd") {
        this->startFullPicBoard();
        this->setBoardType(FULLPICPROOF);
    } //聚类校对
    else if (this->nodeCode == "jljd") {
        this->startClusterBoard();
        this->setBoardType(CLUSTERPROOF);
    }
}

/**
 * @brief MainWindow::addMenuAndBar 加入menubar等
 */
void MainWindow::addMenuAndBar()
{
    fileMenu = new QMenu(QStringLiteral("文件"));
    aboutMenu = new QMenu(QStringLiteral("关于"));
    this->ui->menuBar->addMenu(fileMenu);
    this->ui->menuBar->addMenu(aboutMenu);
    this->ui->menuBar->setStyleSheet("background-color:#eeeeee;");
    clearSqlAction = new QAction(QStringLiteral("清除缓存"));
    quitAction = new QAction(QStringLiteral("退出"));
    aboutAction = new QAction(QStringLiteral("关于同文古籍数字加工平台"));
    fileMenu->addAction(clearSqlAction);
    fileMenu->addAction(quitAction);
    aboutMenu->addAction(aboutAction);
}

/**
 * @brief MainWindow::addToolbarAction 给toolbar上添加Action
 */
void MainWindow::addToolbarAction()
{
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    requestAction = new QAction(QIcon("./images/download"),QStringLiteral("申请"));
    uploadAction = new QAction(QIcon("./images/upload"),QStringLiteral("上传"));

    zoomOutAction = new QAction(QIcon("./images/zoomout"),QStringLiteral("缩小"));
    zoomInAction =  new QAction(QIcon("./images/zoomin"),QStringLiteral("放大"));
    setCoverAction =  new QAction(QIcon("./images/cover"),QStringLiteral("覆盖检查"));
    zoomOutAction->setEnabled(false);
    zoomInAction->setEnabled(false);
    setCoverAction->setEnabled(false);

    mainBodyAction = new QAction(QIcon("./images/mainbody"),QStringLiteral("正文"));
    imageAction = new QAction(QIcon("./images/image"),QStringLiteral("图像"));
    annotationAction = new QAction(QIcon("./images/anno"),QStringLiteral("注释"));
    tableAction = new QAction(QIcon("./images/table"),QStringLiteral("表格"));
    singleRowAction = new QAction(QIcon("./images/singlerow"),QStringLiteral("单行"));
    doubleRowAction = new QAction(QIcon("./images/doublerow"),QStringLiteral("双行"));
    bigCharAction = new QAction(QIcon("./images/bigchar"),QStringLiteral("大字"));
    smallCharAction = new QAction(QIcon("./images/smallchar"),QStringLiteral("小字"));

    mainBodyAction->setEnabled(false);
    imageAction->setEnabled(false);
    annotationAction->setEnabled(false);
    tableAction->setEnabled(false);
    singleRowAction->setEnabled(false);
    doubleRowAction->setEnabled(false);
    bigCharAction->setEnabled(false);
    smallCharAction->setEnabled(false);

    ui->mainToolBar->addAction(requestAction);
    ui->mainToolBar->addAction(uploadAction);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(zoomOutAction);
    ui->mainToolBar->addAction(zoomInAction);
    ui->mainToolBar->addAction(setCoverAction);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(mainBodyAction);
    ui->mainToolBar->addAction(singleRowAction);
    ui->mainToolBar->addAction(bigCharAction);
    ui->mainToolBar->addAction(imageAction);
    ui->mainToolBar->addAction(tableAction);
    ui->mainToolBar->addAction(annotationAction);
    ui->mainToolBar->addAction(doubleRowAction);
    ui->mainToolBar->addAction(smallCharAction);

    mainBodyAction->setCheckable(true);
    singleRowAction->setCheckable(true);
    bigCharAction->setCheckable(true);
    imageAction->setCheckable(true);
    annotationAction->setCheckable(true);
    tableAction->setCheckable(true);
    doubleRowAction->setCheckable(true);
    smallCharAction->setCheckable(true);

    //这个东西是中间分割的那个线
    ui->mainToolBar->addSeparator();

    broadcastNumberEdit->setFixedSize(30,30);
    ui->mainToolBar->addWidget(broadcastNumberEdit);
}
//***************************************************************************toolbar的信号和槽
/**
 * @brief MainWindow::requestActionResponse  申请时候的信号槽
 */
void MainWindow::requestActionResponse()
{
    switch (this->BoardType) {
    case EDITBOARD:
        editboard->on_requestButton_clicked();
        break;
    case PROOFBOARD:
        proofboard->on_requestButton_clicked();
        break;
    case CLUSTERPROOF:
        clustproof->on_requestButton_clicked();
        break;
    case FULLPICPROOF:
        wholepicboard->on_requestButton_clicked();
        break;
    default:
        break;
    }
}

void MainWindow::uploadActionResponse()
{
    switch (this->BoardType) {
    case EDITBOARD:
        editboard->on_getHocrButtom_clicked();
        break;
    case PROOFBOARD:
        proofboard->on_getHocrButtom_clicked();
        break;
    case CLUSTERPROOF:
        clustproof->on_upDateButtom_clicked();
        break;
    case FULLPICPROOF:
        wholepicboard->on_getHocrButtom_clicked();
        break;
    default:
        break;
    }
}

void MainWindow::zoomOutActionResponse()
{
    switch (this->BoardType) {
    case EDITBOARD:
        editboard->on_zoomOutButton_clicked();
        break;
    case PROOFBOARD:
        proofboard->on_zoomOutButton_clicked();
        break;
    case CLUSTERPROOF:
        break;
    case FULLPICPROOF:
        wholepicboard->on_zoomOutButton_clicked();
        break;
    default:
        break;
    }
}

void MainWindow::zoomInActionResponse()
{
    switch (this->BoardType) {
    case EDITBOARD:
        editboard->on_zoomInButton_clicked();
        break;
    case PROOFBOARD:
        proofboard->on_zoomInButton_clicked();
        break;
    case CLUSTERPROOF:
        break;
    case FULLPICPROOF:
        wholepicboard->on_zoomInButton_clicked();
        break;
    default:
        break;
    }
}

void MainWindow::setCoverActionResponse()
{
    switch (this->BoardType) {
    case EDITBOARD:
        editboard->on_setColorButton_clicked();
        break;
    case PROOFBOARD:
        proofboard->on_setColorButton_clicked();
        break;
    case CLUSTERPROOF:
        break;
    case FULLPICPROOF:
        //wholepicboard->on_setColorButton_clicked();
        break;
    default:
        break;
    }
}

void MainWindow::mainBodyActionResponse()
{
    //mainBodyAction->setChecked(false);
    singleRowAction->setChecked(false);
    bigCharAction->setChecked(false);
    imageAction->setChecked(false);
    annotationAction->setChecked(false);
    tableAction->setChecked(false);
    doubleRowAction->setChecked(false);
    smallCharAction->setChecked(false);

    switch (this->BoardType) {
    case EDITBOARD:
        clearEditboardsItem();
        editboard->mScene->setMode(ManageMode::EDIT);
        editboard->mView->setFocus();
        editboard->setMainBodyFlag();
        editboard->mScene->isCreateOrIsHover = false;
        break;
    case PROOFBOARD:
        clearProofboardsItem();
        proofboard->mScene->setMode(ManageMode::EDIT);
        proofboard->mView->setFocus();
        proofboard->setMainBodyFlag();
        proofboard->mScene->isCreateOrIsHover = false;
        break;
    }
}

void MainWindow::imageActionResponse()
{
    mainBodyAction->setChecked(false);
    singleRowAction->setChecked(false);
    bigCharAction->setChecked(false);
    //imageAction->setChecked(false);
    annotationAction->setChecked(false);
    tableAction->setChecked(false);
    doubleRowAction->setChecked(false);
    smallCharAction->setChecked(false);

    switch (this->BoardType) {
    case EDITBOARD:
        clearEditboardsItem();
        editboard->mScene->setMode(ManageMode::EDIT);
        editboard->mView->setFocus();
        editboard->setImageFlag();
        editboard->mScene->isCreateOrIsHover = false;
        break;
    case PROOFBOARD:
        clearProofboardsItem();
        proofboard->mScene->setMode(ManageMode::EDIT);
        proofboard->mView->setFocus();
        proofboard->setImageFlag();
        proofboard->mScene->isCreateOrIsHover = false;
        break;
    }

}

void MainWindow::annotationActionResponse()
{
    mainBodyAction->setChecked(false);
    singleRowAction->setChecked(false);
    bigCharAction->setChecked(false);
    imageAction->setChecked(false);
    //annotationAction->setChecked(false);
    tableAction->setChecked(false);
    doubleRowAction->setChecked(false);
    smallCharAction->setChecked(false);

    //    switch (this->BoardType) {
    //    case EDITBOARD:
    //        clearEditboardsItem();
    //        editboard->mScene->setMode(ManageMode::EDIT);
    //        editboard->mScene->setPenColorAndMode(ANNOTATION);
    //        editboard->mView->setFocus();
    //        break;
    //    case PROOFBOARD:
    //        clearProofboardsItem();
    //        proofboard->mScene->setMode(ManageMode::EDIT);
    //        proofboard->mScene->setPenColorAndMode(ANNOTATION);
    //        proofboard->mView->setFocus();
    //        break;
    //    }
}
void MainWindow::tableActionResponse()
{
    mainBodyAction->setChecked(false);
    singleRowAction->setChecked(false);
    bigCharAction->setChecked(false);
    imageAction->setChecked(false);
    annotationAction->setChecked(false);
    //tableAction->setChecked(false);
    doubleRowAction->setChecked(false);
    smallCharAction->setChecked(false);

    switch (this->BoardType) {
    case EDITBOARD:
        clearEditboardsItem();
        editboard->mScene->setMode(ManageMode::EDIT);
        editboard->mView->setFocus();
        editboard->setTableFlag();
        editboard->mScene->isCreateOrIsHover = false;
        break;
    case PROOFBOARD:
        clearProofboardsItem();
        proofboard->mScene->setMode(ManageMode::EDIT);
        proofboard->mView->setFocus();
        proofboard->setTableFlag();
        proofboard->mScene->isCreateOrIsHover = false;
        break;
    }

}
void MainWindow::singleRowActionResponse()
{
    mainBodyAction->setChecked(false);
    //singleRowAction->setChecked(false);
    bigCharAction->setChecked(false);
    imageAction->setChecked(false);
    annotationAction->setChecked(false);
    tableAction->setChecked(false);
    doubleRowAction->setChecked(false);
    smallCharAction->setChecked(false);

    switch (this->BoardType) {
    case EDITBOARD:
        clearEditboardsItem();
        editboard->mScene->setMode(ManageMode::EDIT);
        editboard->mView->setFocus();
        editboard->setSingleRowFlag();
        editboard->mScene->isCreateOrIsHover = false;
        break;
    case PROOFBOARD:
        clearProofboardsItem();
        proofboard->mScene->setMode(ManageMode::EDIT);
        proofboard->mView->setFocus();
        proofboard->setSingleRowFlag();
        proofboard->mScene->isCreateOrIsHover = false;
        break;
    }
}
void MainWindow::doubleRowActionResponse()
{
    mainBodyAction->setChecked(false);
    singleRowAction->setChecked(false);
    bigCharAction->setChecked(false);
    imageAction->setChecked(false);
    annotationAction->setChecked(false);
    tableAction->setChecked(false);
    //doubleRowAction->setChecked(false);
    smallCharAction->setChecked(false);

    switch (this->BoardType) {
    case EDITBOARD:
        clearEditboardsItem();
        editboard->mScene->setMode(ManageMode::EDIT);
        editboard->mView->setFocus();
        editboard->setDoubleRowFlag();
        editboard->mScene->isCreateOrIsHover = false;
        break;
    case PROOFBOARD:
        clearProofboardsItem();
        proofboard->mScene->setMode(ManageMode::EDIT);
        proofboard->mView->setFocus();
        proofboard->setDoubleRowFlag();
        proofboard->mScene->isCreateOrIsHover = false;
        break;
    }
}
void MainWindow::bigCharActionResponse()
{
    mainBodyAction->setChecked(false);
    singleRowAction->setChecked(false);
    //bigCharAction->setChecked(false);
    imageAction->setChecked(false);
    annotationAction->setChecked(false);
    tableAction->setChecked(false);
    doubleRowAction->setChecked(false);
    smallCharAction->setChecked(false);

    switch (this->BoardType) {
    case EDITBOARD:
        clearEditboardsItem();
        editboard->mScene->setMode(ManageMode::EDIT);
        editboard->mView->setFocus();
        editboard->setBigCharFlag();
        editboard->mScene->isCreateOrIsHover = false;
        break;
    case PROOFBOARD:
        clearProofboardsItem();
        proofboard->mScene->setMode(ManageMode::EDIT);
        proofboard->mView->setFocus();
        //设置flag
        proofboard->setBigCharFlag();
        proofboard->mScene->isCreateOrIsHover = false;
        break;
    }
}
void MainWindow::smallCharActionResponse()
{
    mainBodyAction->setChecked(false);
    singleRowAction->setChecked(false);
    bigCharAction->setChecked(false);
    imageAction->setChecked(false);
    annotationAction->setChecked(false);
    tableAction->setChecked(false);
    doubleRowAction->setChecked(false);

    switch (this->BoardType) {
    case EDITBOARD:
        clearEditboardsItem();
        editboard->mScene->setMode(ManageMode::EDIT);
        editboard->mView->setFocus();
        editboard->setSmallCharFlag();
        editboard->mScene->isCreateOrIsHover = false;
        break;
    case PROOFBOARD:
        clearProofboardsItem();
        proofboard->mScene->setMode(ManageMode::EDIT);
        proofboard->mView->setFocus();
        proofboard->setSmallCharFlag();
        proofboard->mScene->isCreateOrIsHover = false;
        break;
    }
}

//***********************************************************************

void MainWindow::showToolBarAction()
{

    if(BoardType == EDITBOARD || BoardType == PROOFBOARD){
        zoomOutAction->setEnabled(true);
        zoomInAction->setEnabled(true);
        setCoverAction->setEnabled(true);

        mainBodyAction->setEnabled(true);
        imageAction->setEnabled(true);
        annotationAction->setEnabled(true);
        tableAction->setEnabled(true);
        singleRowAction->setEnabled(true);
        doubleRowAction->setEnabled(true);
        bigCharAction->setEnabled(true);
        smallCharAction->setEnabled(true);
        if(BoardType == EDITBOARD){
            editboard->setMainBodyFlag();
            mainBodyAction->setChecked(true);
            imageAction->setChecked(false);
            annotationAction->setChecked(false);
            tableAction->setChecked(false);
            singleRowAction->setChecked(false);
            doubleRowAction->setChecked(false);
            bigCharAction->setChecked(false);
            smallCharAction->setChecked(false);
        } else {
            proofboard->setBigCharFlag();
            bigCharAction->setChecked(true);
            mainBodyAction->setChecked(false);
            imageAction->setChecked(false);
            annotationAction->setChecked(false);
            tableAction->setChecked(false);
            singleRowAction->setChecked(false);
            doubleRowAction->setChecked(false);
            smallCharAction->setChecked(false);
        }
    }
    if(BoardType == FULLPICPROOF)
    {
        zoomOutAction->setEnabled(true);
        zoomInAction->setEnabled(true);
    }
}

/**
 * @brief MainWindow::clearEditboardsItem  消灭editboard那里的透明的item的
 */
void MainWindow::clearEditboardsItem()
{
    for(auto i : editboard->mScene->items()) {
        if(i->isSelected()){
            i->setFlag(QGraphicsItem::ItemIsMovable,false);
            i->setOpacity(1);
            ((MyItem*)i)->setBrush(Qt::transparent);
        }
    }
}

/**
 * @brief MainWindow::clearProofboardsItem  消灭poofboard那里的透明的item的
 */
void MainWindow::clearProofboardsItem()
{
    for(auto i : proofboard->mScene->items()) {
        if(i->isSelected()){
            i->setFlag(QGraphicsItem::ItemIsMovable,false);
            i->setOpacity(1);
            ((MyItem*)i)->setBrush(Qt::transparent);
        }
    }
}


